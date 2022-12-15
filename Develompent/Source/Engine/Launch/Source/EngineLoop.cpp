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
#include "Misc/CommandLine.h"
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
#include "System/Name.h"
#include "LEBuild.h"

#if USE_THEORA_CODEC
#include "System/FullScreenMovieTheora.h"
#else
#include "System/FullScreenMovieFallback.h"
#endif // USE_THEORA_CODEC

#if WITH_EDITOR
#include "Commandlets/BaseCommandlet.h"
#include "Misc/WorldEdGlobals.h"
#endif // WITH_EDITOR

/**
 *	Returns the path to the cooked data for the given platform
 *
 *	@param InPlatform The platform of interest
 *	@param OutPath The path to the cooked content for that platform
 */
void appGetCookedContentPath( EPlatformType InPlatform, std::wstring& OutPath )
{
#if !WITH_EDITOR
	OutPath = appGameDir() + PATH_SEPARATOR + TEXT( "Cooked" ) + appPlatformTypeToString( InPlatform ) + PATH_SEPARATOR;
#else
	OutPath = appGameDir() + PATH_SEPARATOR + TEXT( "Content" ) + PATH_SEPARATOR;
#endif // !WITH_EDITOR
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

void CEngineLoop::InitConfigs()
{
	// Init configs
	GConfig.Init();

	// Set from config max tick rate
	GUseMaxTickRate			= GConfig.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "UseMaxTickRate" ) ).GetBool();

#if WITH_EDITOR
	// Fill table for convert from text to ESurfaceType
	{
		std::vector< CConfigValue >		configSurfaceNames	= GConfig.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Surfaces" ) ).GetArray();
		for ( uint32 index = 0, count = configSurfaceNames.size(); index < count; ++index )
		{
			const CConfigValue&		configSurface			= configSurfaceNames[ index ];
			check( configSurface.GetType() == CConfigValue::T_Object );
			CConfigObject			objectSurface = configSurface.GetObject();

			std::wstring		name		= objectSurface.GetValue( TEXT( "Name" ) ).GetString();
			int32				surfaceID	= objectSurface.GetValue( TEXT( "ID" ) ).GetInt();
			check( surfaceID < ST_Max );
			GSurfaceTypeNames.push_back( std::make_pair( name, ( ESurfaceType )surfaceID ) );
		}
	}

	// Is need cook editor content (include dev content)
	GIsCookEditorContent			= GConfig.GetValue( CT_Editor, TEXT( "Editor.CookPackages" ), TEXT( "CookEditorContent" ) ).GetBool();
	if ( GIsCookEditorContent )
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Enabled cook editor content" ) );
	}

	// Is allow shader debug dump
	GAllowDebugShaderDump			= GConfig.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "AllowShaderDebugDump" ) ).GetBool();
#endif // WITH_EDITOR
}

#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"

/**
 * Pre-Initialize the main loop
 */
int32 CEngineLoop::PreInit( const tchar* InCmdLine )
{
	GGameThreadId = appGetCurrentThreadId();
	GGameName = ANSI_TO_TCHAR( GAMENAME );
	appGetCookedContentPath( GPlatform, GCookedDir );
	
	GCommandLine.Init( InCmdLine );
	CName::StaticInit();
	InitConfigs();

#if WITH_EDITOR
	GIsEditor = GCommandLine.HasParam( TEXT( "editor" ) );
	
	CCommandLine::Values_t	paramValues = GCommandLine.GetValues( TEXT( "commandlet" ) );
	if ( !paramValues.empty() )
	{
		GIsCooker		= paramValues[0] == TEXT( "CookPackages" ) || paramValues[0] == TEXT( "CCookPackagesCommandlet" );
		GIsCommandlet	= !GIsCooker;
	}

	GIsGame = !GIsEditor && !GIsCooker && !GIsCommandlet;
#endif // WITH_EDITOR

	GLog->Init();
	int32		result = appPlatformPreInit();
	
	// Loading table of contents
	if ( !GIsEditor && !GIsCooker )
	{
		std::wstring	tocPath = GCookedDir + PATH_SEPARATOR + CTableOfContets::GetNameTOC();

#if WITH_EDITOR
		if ( !GFileSystem->IsExistFile( tocPath ) )
		{
			CCommandLine		commandLine;
			commandLine.Init( TEXT( "-commandlet=CookerSync" ) );
			CBaseCommandlet::ExecCommandlet( commandLine );
		}
#endif // WITH_EDITOR
		
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

	LE_LOG( LT_Log, LC_Init, TEXT( "User: %s//%s" ), appComputerName().c_str(), appUserName().c_str() );
	LE_LOG( LT_Log, LC_Init, TEXT( "Started with arguments: %s" ), InCmdLine );

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "CBaseEngine" );
		if ( !GIsEditor )
		{
			classEngineName = GConfig.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "Class" ) ).GetString().c_str();
		}
#if WITH_EDITOR
		else
		{
			classEngineName = GConfig.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Class" ) ).GetString().c_str();
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
int32 CEngineLoop::Init()
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

#if WITH_EDITOR
	check( !GIsEditor || GIsEditor && GEditorEngine );
#endif // WITH_EDITOR

	// Start render thread
	StartRenderingThread();

	// Playing startup movies (only for game)
	if ( GIsGame )
	{
		GFullScreenMovie->GameThreadInitiateStartupSequence();
	}

	// Parse cmd line for start commandlets
#if WITH_EDITOR
	if ( CBaseCommandlet::ExecCommandlet( GCommandLine ) )
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
		CConfigValue		configEditorStartupMap = GConfig.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "EditorStartupMap" ) );
		if ( configEditorStartupMap.IsA( CConfigValue::T_String ) )
		{
			map = configEditorStartupMap.GetString();
		}
	}
	else
#endif // WITH_EDITOR
	{
		// Get map for loading in game
		CConfigValue		configGameDefaultMap = GConfig.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "GameDefaultMap" ) );
		if ( configGameDefaultMap.IsA( CConfigValue::T_String ) )
		{
			map = configGameDefaultMap.GetString();
		}
	}

	// If command line has param 'map', we load she
	if ( GCommandLine.HasParam( TEXT( "map" ) ) )
	{
		map = GCommandLine.GetFirstValue( TEXT( "map" ) );
	}

	if ( !map.empty() )
	{
		appSetSplashText( STT_StartupProgress, CString::Format( TEXT( "Loading map '%s'..." ), map.c_str() ).c_str() );

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
		GFullScreenMovie->GameThreadSetSkippable( true );		// After loading map we allow skip startup movies sequence
		GFullScreenMovie->GameThreadStopMovie();
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
	GConfig.Shutdown();
	GCommandLine.Shutdown();
}
