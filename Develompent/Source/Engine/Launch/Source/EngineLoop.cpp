#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "System/Config.h"
#include "System/ThreadingBase.h"
#include "System/InputSystem.h"
#include "System/Package.h"
#include "Containers/String.h"
#include "Misc/Class.h"
#include "Math/Color.h"
#include "Scripts/ScriptEngine.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "UIEngine.h"
#include "Misc/UIGlobals.h"
#include "EngineLoop.h"
#include "Render/RenderingThread.h"
#include "System/SplashScreen.h"
#include "System/BaseEngine.h"

#if WITH_EDITOR
#include "Commandlets/BaseCommandlet.h"
#endif // WITH_EDITOR

/**
 * Update GCurrentTime and GDeltaTime while taking into account max tick rate
 */
void appUpdateTimeAndHandleMaxTickRate()
{
	GLastTime = GCurrentTime;
	GCurrentTime = appSeconds();
	GDeltaTime = GCurrentTime - GLastTime;

	float		maxTickRate = Max( GEngine->GetMaxTickRate(), 1.f );
	appSleep( Max( 1.f / maxTickRate - GDeltaTime, 0.0 ) );		// BS yehor.pohuliaka - It is possible that you need to somehow fix the stalling of the render thread another way
}

/**
 * Constructor
 */
FEngineLoop::FEngineLoop() :
	isInitialize( false )
{}

/**
 * Destructor
 */
FEngineLoop::~FEngineLoop()
{}

/**
 * Serialize configs
 */
void FEngineLoop::SerializeConfigs()
{
	// Loading engine config
	FArchive*		arConfig = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "Config/Engine.json" ) );
	if ( arConfig )
	{
		GEngineConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading input config
	arConfig = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "Config/Input.json" ) );
	if ( arConfig )
	{
		GInputConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading game config
	arConfig = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "Config/Game.json" ) );
	if ( arConfig )
	{
		GGameConfig.Serialize( *arConfig );
		delete arConfig;
	}

	GGameName = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "ShortName" ) ).GetString();

	// Loading editor config
#if WITH_EDITOR
	arConfig = GFileSystem->CreateFileReader( appBaseDir() + TEXT( "Config/Editor.json" ) );
	if ( arConfig )
	{
		GEditorConfig.Serialize( *arConfig );
		delete arConfig;
	}
#endif // WITH_EDITOR
}

/**
 * Pre-Initialize the main loop
 */
int32 FEngineLoop::PreInit( const tchar* InCmdLine )
{
	GGameThreadId = appGetCurrentThreadId();
	SerializeConfigs();

#if WITH_EDITOR
	GIsEditor = appParseParam( InCmdLine, TEXT( "editor" ) );
#endif // WITH_EDITOR

	GLog->Init();

	int32		result = appPlatformPreInit( InCmdLine );
	GScriptEngine->Init();
	GRHI->Init( GIsEditor );

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "LBaseEngine" );
		if ( !GIsEditor )
		{
			classEngineName = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "Class" ) ).GetString().c_str();
		}
#if WITH_EDITOR
		else
		{
			classEngineName = GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Class" ) ).GetString().c_str();
		}
#endif // WITH_EDITOR

		const LClass*		lclass = LClass::StaticFindClass( classEngineName.c_str() );
		checkMsg( lclass, TEXT( "Class engine %s not found" ), classEngineName.c_str() );
		
		GEngine = lclass->CreateObject< LBaseEngine >();
		check( GEngine );
	}

	// Parse cmd line for start commandlets
#if WITH_EDITOR
	{
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCmdLine, tokens, switches );

		// If tokens more two we creating commandlet and execute it
		// PS: 0 index - path to exe file, 1 index - name commandlet
		if ( tokens.size() >= 2 )
		{
			const std::wstring&			nameCommandlet = tokens[ 1 ];
			LClass*						lclassCommandlet = LClass::StaticFindClass( nameCommandlet.c_str() );

			// If class not found try to search by added 'L' in prefix and 'Commandlet' in sufix
			if ( !lclassCommandlet )
			{
				lclassCommandlet = LClass::StaticFindClass( ( std::wstring( TEXT( "L" ) ) + nameCommandlet + TEXT( "Commandlet" ) ).c_str() );
			}

			// Create and execute commandlet
			if ( lclassCommandlet )
			{
				LBaseCommandlet*			commandlet = lclassCommandlet->CreateObject< LBaseCommandlet >();
				check( commandlet );
				LE_LOG( LT_Log, LC_Commandlet, TEXT( "Started commandlet '%s'" ), nameCommandlet.c_str() );
				commandlet->Main( InCmdLine );
				delete commandlet;

				GIsRequestingExit = true;
				return result;
			}
		}
	}
#endif // WITH_EDITOR

	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );
	return result;
}

/**
 * Initialize the main loop
 */
int32 FEngineLoop::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: " ENGINE_VERSION_STRING ) );

	appSetSplashText( STT_StartupProgress, TEXT( "Init platform" ) );
	int32		result = appPlatformInit();
	check( !result );

	appSetSplashText( STT_StartupProgress, TEXT( "Init package manager" ) );
	GPackageManager->Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init shaders" ) );
	GShaderManager->Init();
	//GUIEngine->Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init engine" ) );
	GEngine->Init();

	// Loading map
	std::wstring		map;

#if WITH_EDITOR
	if ( GIsEditor )
	{
		// Get map for loading in editor
		FConfigValue		configEditorStartupMap = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "EditorStartupMap" ) );
		if ( configEditorStartupMap.IsA( FConfigValue::T_String ) )
		{
			map = configEditorStartupMap.GetString();
		}
	}
	else
#endif // WITH_EDITOR
	{
		// Get map for loading in game
		FConfigValue		configGameDefaultMap = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "GameDefaultMap" ) );
		if ( configGameDefaultMap.IsA( FConfigValue::T_String ) )
		{
			map = configGameDefaultMap.GetString();
		}
	}

	if ( !map.empty() )
	{
		appSetSplashText( STT_StartupProgress, FString::Format( TEXT( "Loading map '%s'..." ), map.c_str() ).c_str() );

		std::wstring		error;
		bool				successed = GEngine->LoadMap( map, error );
		if ( !successed )
		{
#if WITH_EDITOR
			if ( GIsEditor )
			{
				LE_LOG( LT_Warning, LC_Init, TEXT( "Failed loading map '%s'. Error: %s" ), map.c_str(), error.c_str() );
			}
			else
#endif // WITH_EDITOR
			{
				appErrorf( TEXT( "Failed loading map '%s'. Error: %s" ), map.c_str(), error.c_str() );
				result = 2;
			}
		}
	}
	else if ( !GIsEditor )
	{
		appErrorf( TEXT( "In game config not setted or not valid default map (parameter 'GameDefaultMap')" ) );
		result = 1;
	}

	// Start render thread
	StartRenderingThread();
	return result;
}

/**
 * Process event
 */
void FEngineLoop::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Handling system events
	GEngine->ProcessEvent( InWindowEvent );
}

/**
 * Advances main loop
 */
void FEngineLoop::Tick()
{
	appUpdateTimeAndHandleMaxTickRate();

	// Update package manager
	GPackageManager->Tick();
	
	// Update engine
	GEngine->Tick( GDeltaTime );

	// Reset input events after game frame
	GInputSystem->ResetEvents();
}

/**
 * Performs shut down
 */
void FEngineLoop::Exit()
{
	StopRenderingThread();

	GEngine->Shutdown();
	delete GEngine;
	GEngine = nullptr;

	//GUIEngine->Shutdown();
	GShaderManager->Shutdown();
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}