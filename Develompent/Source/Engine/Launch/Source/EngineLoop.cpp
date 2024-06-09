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
#include "System/Threading.h"
#include "System/InputSystem.h"
#include "System/Package.h"
#include "System/AudioEngine.h"
#include "Misc/StringConv.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/ObjectGlobals.h"
#include "Math/Color.h"
#include "Misc/CommandLine.h"
#include "Misc/TableOfContents.h"
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
#include "System/Cvar.h"
#include "System/Name.h"
#include "System/System.h"
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

/*
==================
Sys_GetCookedContentPath
==================
*/
void Sys_GetCookedContentPath( EPlatformType InPlatform, std::wstring& OutPath )
{
	// Returns the path to the cooked data for the given platform
#if WITH_EDITOR
	if ( !g_IsCooker && !g_CommandLine.HasParam( TEXT( "usecooked" ) ) )
	{
		OutPath = Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content" ) + PATH_SEPARATOR;
	}
	else
#endif // WITH_EDITOR
	{
		OutPath = Sys_GameDir() + PATH_SEPARATOR + TEXT( "Cooked" ) + Sys_PlatformTypeToString( InPlatform ) + PATH_SEPARATOR;
	}
}

/*
==================
Sys_UpdateTimeAndHandleMaxTickRate
==================
*/
void Sys_UpdateTimeAndHandleMaxTickRate()
{
	// Update g_CurrentTime and g_DeltaTime while taking into account max tick rate
	g_LastTime = g_CurrentTime;
	g_CurrentTime = Sys_Seconds();
	g_DeltaTime = g_CurrentTime - g_LastTime;

	if ( g_UseMaxTickRate )
	{
		float		maxTickRate = Max( g_Engine->GetMaxTickRate(), 1.f );
		Sys_Sleep( Max( 1.f / maxTickRate - g_DeltaTime, 0.0 ) );		// BS yehor.pohuliaka - It is possible that you need to somehow fix the stalling of the render thread another way
	}
}

/*
==================
Sys_InitFullScreenMoviePlayer
==================
*/
void Sys_InitFullScreenMoviePlayer()
{
	// Initialize the global full screen movie player
	Assert( !g_FullScreenMovie );

#if USE_THEORA_CODEC
	g_FullScreenMovie = new CFullScreenMovieTheora();
#else
	g_FullScreenMovie = new CFullScreenMovieFallback();
#endif // USE_THEORA_CODEC

	Assert( g_FullScreenMovie );
}


/*
==================
CEngineLoop::CEngineLoop
==================
*/
CEngineLoop::CEngineLoop() 
	: isInitialize( false )
	, bIsFocus( true )
{}

/*
==================
CEngineLoop::~CEngineLoop
==================
*/
CEngineLoop::~CEngineLoop()
{}

/*
==================
CEngineLoop::InitConfigs
==================
*/
void CEngineLoop::InitConfigs()
{
	// Init configs
	CConfig::Get().Init();

	// Set from config max tick rate
	const CJsonValue*			configUseMaxTickRate = CConfig::Get().GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "UseMaxTickRate" ) );
	g_UseMaxTickRate			= configUseMaxTickRate ? configUseMaxTickRate->GetBool() : false;

#if WITH_EDITOR
	// Fill table for convert from text to ESurfaceType
	{
		const CJsonValue*				configSurfaceNamesValue = CConfig::Get().GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Surfaces" ) );
		const std::vector<CJsonValue>*	configSurfaceNames = configSurfaceNamesValue ? configSurfaceNamesValue->GetArray() : nullptr;
		if ( configSurfaceNames )
		{
			for ( uint32 index = 0, count = configSurfaceNames->size(); index < count; ++index )
			{
				const CJsonValue&		configSurface = configSurfaceNames->at( index );
				const CJsonObject*		objectSurface = configSurface.GetObject();
				if ( !objectSurface )
				{
					Warnf( TEXT( "Invalid 'Editor.Editor:Surfaces[%i]'\n" ), index );
					continue;
				}

				const CJsonValue*	configName = objectSurface->GetValue( TEXT( "Name" ) );
				const CJsonValue*	configID = objectSurface->GetValue( TEXT( "ID" ) );
				if ( !configName || !configID )
				{
					Warnf( TEXT( "Invalid 'Editor.Editor:Surfaces[%i]'\n" ), index );
					continue;
				}

				std::wstring name		= configName->GetString();
				int32		 surfaceID	= configID->GetInt();
				Assert( surfaceID < ST_Max );
				g_SurfaceTypeNames.push_back( std::make_pair( name, ( ESurfaceType )surfaceID ) );
			}
		}
	}

	// Is need cook editor content (include dev content)
	const CJsonValue*		configCookEditorContent = CConfig::Get().GetValue( CT_Editor, TEXT( "Editor.CookPackages" ), TEXT( "CookEditorContent" ) );
	g_IsCookEditorContent	= configCookEditorContent ? configCookEditorContent->GetBool() : nullptr;
	if ( g_IsCookEditorContent )
	{
		Warnf( TEXT( "Enabled cook editor content\n" ) );
	}

	// Is allow shader debug dump
	const CJsonValue*		configAllowShaderDebugDump = CConfig::Get().GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "AllowShaderDebugDump" ) );
	g_AllowDebugShaderDump	= ( configAllowShaderDebugDump ? configAllowShaderDebugDump->GetBool() : false ) || g_CommandLine.HasParam( TEXT( "-shaderdump" ) );
#endif // WITH_EDITOR
}

#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"

#include "Reflection/ObjectPackage.h"
#include "Reflection/Class.h"
#include "Reflection/Object.h"
#include "Reflection/Property.h"
#include "Actors/StaticMesh.h"

/*
==================
CEngineLoop::PreInit
==================
*/
int32 CEngineLoop::PreInit( const tchar* InCmdLine )
{
	g_GameThreadId = Sys_GetCurrentThreadId();
	g_GameName = ANSI_TO_TCHAR( GAMENAME );

	g_CommandLine.Init( InCmdLine );
	CName::StaticInit();
	InitConfigs();	

#if WITH_EDITOR
	g_IsEditor = g_CommandLine.HasParam( TEXT( "editor" ) );
	
	CCommandLine::Values_t	paramValues = g_CommandLine.GetValues( TEXT( "commandlet" ) );
	if ( !paramValues.empty() )
	{
		g_IsCooker			= !L_Stricmp( paramValues[0].c_str(), TEXT( "CookPackages" ) ) || !L_Stricmp( paramValues[0].c_str(), TEXT( "CCookPackagesCommandlet" ) );
		g_IsCommandlet		= !g_IsCooker;
	}

	g_IsGame = !g_IsEditor && !g_IsCooker && !g_IsCommandlet;
#endif // WITH_EDITOR

	Sys_GetCookedContentPath( g_Platform, g_CookedDir );

	g_Log->Init();
	int32		result = Sys_PlatformPreInit();
	
	// Initialize the system
	CSystem::Get().Init();

	// Initialize CObject system
	CObject::StaticInit();

	// Loading table of contents
	if ( !g_IsEditor && !g_IsCooker )
	{
		std::wstring	tocPath = g_CookedDir + PATH_SEPARATOR + CTableOfContets::GetNameTOC();
		CArchive*		archiveTOC	= g_FileSystem->CreateFileReader( tocPath );
		if ( archiveTOC )
		{
			g_TableOfContents.Serialize( *archiveTOC );
			delete archiveTOC;
		}
		else
		{
			Warnf( TEXT( "TOC file '%s' not found..\n" ), tocPath.c_str() );
		}
	}

	if ( !g_IsCooker && !g_IsEditor && !g_FileSystem->IsExistFile( g_CookedDir, true ) )
	{
		Sys_Error( TEXT( "Cooked directory '%s' not exist. For work need cook packages" ), g_CookedDir.c_str() );
		return -1;
	}

	g_Window->Create( ANSI_TO_TCHAR( ENGINE_NAME " " ENGINE_VERSION_STRING ), 1, 1, SW_Default );
	g_RHI->Init( g_IsEditor );

	Logf( TEXT( "User: %s//%s\n" ), Sys_GetComputerName().c_str(), Sys_GetUserName().c_str() );
	Logf( TEXT( "Started with arguments: %s\n" ), InCmdLine );

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "CBaseEngine" );
		if ( !g_IsEditor )
		{
			const CJsonValue*	configEngineClass = CConfig::Get().GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "Class" ) );
			classEngineName		= configEngineClass ? configEngineClass->GetString() : TEXT( "" );
		}
#if WITH_EDITOR
		else
		{
			const CJsonValue*	configEngineClass = CConfig::Get().GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Class" ) );
			classEngineName		= configEngineClass ? configEngineClass->GetString() : TEXT( "" );
		}
#endif // WITH_EDITOR

		const CClass*		lclass = FindObjectFast<CClass>( nullptr, classEngineName, true, true );
		AssertMsg( lclass, TEXT( "Class engine %s not found" ), classEngineName.c_str() );
		
		CObjectPackage*		enginePackage = CObjectPackage::CreatePackage( nullptr, TEXT( "Engine" ) );
		g_Engine			= lclass->CreateObject<CBaseEngine>( enginePackage, NAME_None, OBJECT_Public );
		Assert( g_Engine );
	}

	return result;
}

/*
==================
CEngineLoop::Init
==================
*/
int32 CEngineLoop::Init()
{
	// Init full screen movie player and start startup movies (only if this game)
	Sys_InitFullScreenMoviePlayer();

	Logf( TEXT( "Engine version: " ENGINE_VERSION_STRING "\n" ) );

	Sys_SetSplashText( STT_StartupProgress, TEXT( "Init platform" ) );
	int32		result = Sys_PlatformInit();
	Assert( !result );

	Sys_SetSplashText( STT_StartupProgress, TEXT( "Init package manager" ) );
	g_PackageManager->Init();

	Sys_SetSplashText( STT_StartupProgress, TEXT( "Init shaders" ) );
	g_ShaderManager->Init();

	Sys_SetSplashText( STT_StartupProgress, TEXT( "Init audio" ) );
	g_AudioEngine.Init();

	Sys_SetSplashText( STT_StartupProgress, TEXT( "Init engine" ) );
	g_Engine->Init();

#if WITH_EDITOR
	Assert( !g_IsEditor || g_IsEditor && g_EditorEngine );
#endif // WITH_EDITOR

	// Start render thread
	StartRenderingThread();

	// Playing startup movies (only for game)
	if ( g_IsGame )
	{
		g_FullScreenMovie->GameThreadInitiateStartupSequence();
	}

	// Setup GC optimizations
	CObjectGC::Get().CloseDisregardForGC();
	g_IsInitialLoad = false;

	// Parse cmd line for start commandlets
#if WITH_EDITOR
	if ( CBaseCommandlet::ExecCommandlet( g_CommandLine ) )
	{
		return result;
	}
#endif // WITH_EDITOR

	// Loading map
	std::wstring		map;

#if WITH_EDITOR
	if ( g_IsEditor )
	{
		// Get map for loading in editor
		const CJsonValue*		configEditorStartupMap = CConfig::Get().GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "EditorStartupMap" ) );
		if ( configEditorStartupMap && configEditorStartupMap->IsA( JVT_String ) )
		{
			map = configEditorStartupMap->GetString();
		}
	}
	else
#endif // WITH_EDITOR
	{
		// Get map for loading in game
		const CJsonValue*		configGameDefaultMap = CConfig::Get().GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "GameDefaultMap" ) );
		if ( configGameDefaultMap && configGameDefaultMap->IsA( JVT_String ) )
		{
			map = configGameDefaultMap->GetString();
		}
	}

	// If command line has param 'map', we load she
	if ( g_CommandLine.HasParam( TEXT( "map" ) ) )
	{
		map = g_CommandLine.GetFirstValue( TEXT( "map" ) );
		if ( CFilename( map ).GetExtension().empty() )
		{
			map += TEXT( ".map" );
		}
	}

	// Mark that we start executing in MainLoop()
	g_IsRunning = true;

	if ( !map.empty() )
	{
		Sys_SetSplashText( STT_StartupProgress, L_Sprintf( TEXT( "Loading map '%s'..." ), map.c_str() ).c_str() );

		std::wstring		error;
		bool				bAbsolutePath = L_IsAbsolutePath( map );
		bool				successed = g_Engine->LoadMap( !bAbsolutePath ? L_Sprintf( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s" ), g_CookedDir.c_str(), map.c_str() ) : map, error );
		if ( !successed )
		{
			Sys_Error( TEXT( "Failed loading map '%s'. Error: %s" ), map.c_str(), error.c_str() );
			result = 2;
		}
	}
	else if ( !g_IsEditor )
	{
		Sys_Error( TEXT( "In game config not setted or not valid default map (parameter 'GameDefaultMap')" ) );
		result = 1;
	}

	// Stop playing startup movies (only for game)
	if ( g_IsGame )
	{
		g_FullScreenMovie->GameThreadSetSkippable( true );		// After loading map we allow skip startup movies sequence
		g_FullScreenMovie->GameThreadStopMovie();
	}
	return result;
}

/*
==================
CEngineLoop::ProcessEvent
==================
*/
void CEngineLoop::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	// Handling system events
	g_Engine->ProcessEvent( InWindowEvent );

	// Handling window focus event
	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowFocusGained:
		bIsFocus = true;
		g_AudioDevice.SetMutedDevice( false );
		break;

	case WindowEvent::T_WindowFocusLost:
		bIsFocus = false;
		g_AudioDevice.SetMutedDevice( true );
		break;
	}
}

/*
==================
CEngineLoop::Tick
==================
*/
void CEngineLoop::Tick()
{
	// If we lost focus - not update engine tick
	if ( !bIsFocus )
	{
		return;
	}

	Sys_UpdateTimeAndHandleMaxTickRate();

	// Update package manager
	g_PackageManager->Tick();
	
	// Update engine
	g_Engine->Tick( g_DeltaTime );

	// Reset input events after game frame
	g_InputSystem->ResetEvents();
}

/*
==================
CEngineLoop::Exit
==================
*/
void CEngineLoop::Exit()
{
	g_IsRunning = false;
	StopRenderingThread();

	g_PackageManager->Shutdown();

	g_Engine->Shutdown();
	g_Engine->RemoveFromRoot();
	g_Engine = nullptr;

	delete g_FullScreenMovie;
	g_FullScreenMovie = nullptr;

	g_AudioEngine.Shutdown();
	g_ShaderManager->Shutdown();
	g_RHI->Destroy();

	g_Window->Close();
	CObject::CleanupLinkerMap();
	CObject::StaticExit();
	CSystem::Get().Shutdown();
	g_Log->TearDown();
	CConfig::Get().Shutdown();
	g_CommandLine.Shutdown();
	g_Cvar.UnregisterAllCommands();
}
