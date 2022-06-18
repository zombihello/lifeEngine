#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/AudioGlobals.h"
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
#include "System/AudioEngine.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Misc/Class.h"
#include "Math/Color.h"
#include "Misc/TableOfContents.h"
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
#include "System/FullScreenMovie.h"
#include "LEBuild.h"

#if USE_THEORA_CODEC
#include "System/FullScreenMovieTheora.h"
#else
#include "System/FullScreenMovieFallback.h"
#endif // USE_THEORA_CODEC

#if WITH_EDITOR
#include "Commandlets/BaseCommandlet.h"
#endif // WITH_EDITOR

/**
 *	Returns the path to the cooked data for the given platform
 *
 *	@param InPlatform The platform of interest
 *	@param OutPath The path to the cooked content for that platform
 */
void appGetCookedContentPath( EPlatformType InPlatform, std::wstring& OutPath )
{
	OutPath = appGameDir() + PATH_SEPARATOR + TEXT( "Cooked" ) + appPlatformTypeToString( InPlatform ) + PATH_SEPARATOR;
}

/**
 * Update GCurrentTime and GDeltaTime while taking into account max tick rate
 */
void appUpdateTimeAndHandleMaxTickRate()
{
	GLastTime = GCurrentTime;
	GCurrentTime = appSeconds();
	GDeltaTime = GCurrentTime - GLastTime;

	if ( GUseMaxTickRate )
	{
		float		maxTickRate = Max( GEngine->GetMaxTickRate(), 1.f );
		appSleep( Max( 1.f / maxTickRate - GDeltaTime, 0.0 ) );		// BS yehor.pohuliaka - It is possible that you need to somehow fix the stalling of the render thread another way
	}
}

/**
 * Initialize the global full screen movie player
 */
void appInitFullScreenMoviePlayer()
{
	check( !GFullScreenMovie );

#if USE_THEORA_CODEC
	GFullScreenMovie = new CFullScreenMovieTheora();
#else
	GFullScreenMovie = new CFullScreenMovieFallback();
#endif // USE_THEORA_CODEC

	check( GFullScreenMovie );
}

/**
 * Constructor
 */
CEngineLoop::CEngineLoop() 
	: isInitialize( false )
	, bIsFocus( true )
{}

/**
 * Destructor
 */
CEngineLoop::~CEngineLoop()
{}

/**
 * Serialize configs
 */
void CEngineLoop::SerializeConfigs()
{
	// Loading engine config
	CArchive*		arConfig = GFileSystem->CreateFileReader( appGameDir() + TEXT( "Config/Engine.json" ) );
	if ( arConfig )
	{
		GEngineConfig.Serialize( *arConfig );
		delete arConfig;
	}

	GUseMaxTickRate			= GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "UseMaxTickRate" ) ).GetBool();

	// Loading input config
	arConfig = GFileSystem->CreateFileReader( appGameDir() + TEXT( "Config/Input.json" ) );
	if ( arConfig )
	{
		GInputConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading game config
	arConfig = GFileSystem->CreateFileReader( appGameDir() + TEXT( "Config/Game.json" ) );
	if ( arConfig )
	{
		GGameConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Loading editor config
#if WITH_EDITOR
	arConfig = GFileSystem->CreateFileReader( appGameDir() + TEXT( "Config/Editor.json" ) );
	if ( arConfig )
	{
		GEditorConfig.Serialize( *arConfig );
		delete arConfig;
	}

	// Fill table for convert from text to ESurfaceType
	{
		std::vector< CConfigValue >		configSurfaceNames = GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Surfaces" ) ).GetArray();
		for ( uint32 index = 0, count = configSurfaceNames.size(); index < count; ++index )
		{
			const CConfigValue&		configSurface = configSurfaceNames[ index ];
			check( configSurface.GetType() == CConfigValue::T_Object );
			CConfigObject			objectSurface = configSurface.GetObject();

			std::wstring		name		= objectSurface.GetValue( TEXT( "Name" ) ).GetString();
			int32				surfaceID	= objectSurface.GetValue( TEXT( "ID" ) ).GetInt();
			check( surfaceID < ST_Max );
			GSurfaceTypeNames.push_back( std::make_pair( name, ( ESurfaceType )surfaceID ) );
		}
	}

	// Is need cook editor content (include dev content)
	GIsCookEditorContent			= GEditorConfig.GetValue( TEXT( "Editor.CookPackages" ), TEXT( "CookEditorContent" ) ).GetBool();
	if ( GIsCookEditorContent )
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Enabled cook editor content" ) );
	}

	// Is allow shader debug dump
	GAllowDebugShaderDump			= GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "AllowShaderDebugDump" ) ).GetBool();
#endif // WITH_EDITOR
}

/**
 * Pre-Initialize the main loop
 */
int32 CEngineLoop::PreInit( const tchar* InCmdLine )
{
	GGameThreadId = appGetCurrentThreadId();
	GGameName = ANSI_TO_TCHAR( GAMENAME );
	appGetCookedContentPath( GPlatform, GCookedDir );
	SerializeConfigs();

#if WITH_EDITOR
	GIsEditor = appParseParam( InCmdLine, TEXT( "editor" ) );
	GIsCooker = appParseParam( InCmdLine, TEXT( "CookPackages" ) ) || appParseParam( InCmdLine, TEXT( "CCookPackagesCommandlet" ) );

	// Are we launching a commandlet
	{
		std::vector< std::wstring >		tokens;
		std::vector< std::wstring >		switches;
		appParseCommandLine( InCmdLine, tokens, switches );

		// PS: 0 index - path to exe file, 1 index - name commandlet
		if ( !GIsEditor && tokens.size() >= 2 )
		{
			GIsCommandlet = true;
		}
	}

	GIsGame = !GIsEditor && !GIsCooker && !GIsCommandlet;
#endif // WITH_EDITOR

	GLog->Init();
	int32		result = appPlatformPreInit( InCmdLine );
	
	// Loading table of contents
	if ( !GIsCooker )
	{
		std::wstring	tocPath		= GCookedDir + PATH_SEPARATOR + CTableOfContets::GetNameTOC();
		CArchive*		archiveTOC	= GFileSystem->CreateFileReader( tocPath );
		if ( archiveTOC )
		{
			GTableOfContents.Serialize( *archiveTOC );
			delete archiveTOC;
		}
		else
		{
			LE_LOG( LT_Warning, LC_Package, TEXT( "TOC file '%s' not found.." ), tocPath.c_str() );
		}
	}

	if ( !GIsCooker && !GIsEditor && !GFileSystem->IsExistFile( GCookedDir, true ) )
	{
		appErrorf( TEXT( "Cooked directory '%s' not exist. For work need cook packages" ), GCookedDir.c_str() );
		return -1;
	}

	GWindow->Create( ANSI_TO_TCHAR( ENGINE_NAME " " ENGINE_VERSION_STRING ), 1, 1, SW_Default );
	GScriptEngine->Init();
	GRHI->Init( GIsEditor );

	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "CBaseEngine" );
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

		const CClass*		lclass = CClass::StaticFindClass( classEngineName.c_str() );
		checkMsg( lclass, TEXT( "Class engine %s not found" ), classEngineName.c_str() );
		
		GEngine = lclass->CreateObject< CBaseEngine >();
		check( GEngine );
	}

	return result;
}

/**
 * Initialize the main loop
 */
int32 CEngineLoop::Init( const tchar* InCmdLine )
{
	// Init full screen movie player and start startup movies (only if this game)
	appInitFullScreenMoviePlayer();

	LE_LOG( LT_Log, LC_Init, TEXT( "Engine version: " ENGINE_VERSION_STRING ) );

	appSetSplashText( STT_StartupProgress, TEXT( "Init platform" ) );
	int32		result = appPlatformInit();
	check( !result );

	appSetSplashText( STT_StartupProgress, TEXT( "Init package manager" ) );
	GPackageManager->Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init shaders" ) );
	GShaderManager->Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init audio" ) );
	GAudioEngine.Init();

	appSetSplashText( STT_StartupProgress, TEXT( "Init engine" ) );
	GEngine->Init();

	// Start render thread
	StartRenderingThread();

	// Playing startup movies (only for game)
	if ( GIsGame )
	{
		GFullScreenMovie->GameThreadInitiateStartupSequence();
	}

	// Parse cmd line for start commandlets
#if WITH_EDITOR
	if ( ÑBaseCommandlet::ExecCommandlet( InCmdLine, 1 ) )
	{
		return result;
	}
#endif // WITH_EDITOR

	// Loading map
	std::wstring		map;

#if WITH_EDITOR
	if ( GIsEditor )
	{
		// Get map for loading in editor
		CConfigValue		configEditorStartupMap = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "EditorStartupMap" ) );
		if ( configEditorStartupMap.IsA( CConfigValue::T_String ) )
		{
			map = configEditorStartupMap.GetString();
		}
	}
	else
#endif // WITH_EDITOR
	{
		// Get map for loading in game
		CConfigValue		configGameDefaultMap = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "GameDefaultMap" ) );
		if ( configGameDefaultMap.IsA( CConfigValue::T_String ) )
		{
			map = configGameDefaultMap.GetString();
		}
	}

	if ( !map.empty() )
	{
		appSetSplashText( STT_StartupProgress, ÑString::Format( TEXT( "Loading map '%s'..." ), map.c_str() ).c_str() );

		std::wstring		error;
		bool				successed = GEngine->LoadMap( map, error );
		if ( !successed )
		{
			appErrorf( TEXT( "Failed loading map '%s'. Error: %s" ), map.c_str(), error.c_str() );
			result = 2;
		}
	}
	else if ( !GIsEditor )
	{
		appErrorf( TEXT( "In game config not setted or not valid default map (parameter 'GameDefaultMap')" ) );
		result = 1;
	}

	// Stop playing startup movies (only for game)
	if ( GIsGame )
	{
		GFullScreenMovie->GameThreadStopMovie();
		GWindow->Show();
	}
	return result;
}

/**
 * Process event
 */
void CEngineLoop::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	// Handling system events
	GEngine->ProcessEvent( InWindowEvent );

	// Handling window focus event
	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_WindowFocusGained:
		bIsFocus = true;
		GAudioDevice.SetMutedDevice( false );
		break;

	case SWindowEvent::T_WindowFocusLost:
		bIsFocus = false;
		GAudioDevice.SetMutedDevice( true );
		break;
	}
}

/**
 * Advances main loop
 */
void CEngineLoop::Tick()
{
	// If we lost focus - not update engine tick
	if ( !bIsFocus )
	{
		return;
	}

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
void CEngineLoop::Exit()
{
	StopRenderingThread();

	GPackageManager->Shutdown();

	GEngine->Shutdown();
	delete GEngine;
	GEngine = nullptr;

	delete GFullScreenMovie;
	GFullScreenMovie = nullptr;

	GAudioEngine.Shutdown();
	GShaderManager->Shutdown();
	GRHI->Destroy();

	GWindow->Close();
	GLog->TearDown();
}
