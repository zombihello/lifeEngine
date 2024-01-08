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
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/ObjectIterator.h"
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
	g_Config.Init();

	// Set from config max tick rate
	g_UseMaxTickRate			= g_Config.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "UseMaxTickRate" ) ).GetBool();

#if WITH_EDITOR
	// Fill table for convert from text to ESurfaceType
	{
		std::vector< CConfigValue >		configSurfaceNames	= g_Config.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Surfaces" ) ).GetArray();
		for ( uint32 index = 0, count = configSurfaceNames.size(); index < count; ++index )
		{
			const CConfigValue&		configSurface			= configSurfaceNames[ index ];
			Assert( configSurface.GetType() == CConfigValue::T_Object );
			CConfigObject			objectSurface = configSurface.GetObject();

			std::wstring		name		= objectSurface.GetValue( TEXT( "Name" ) ).GetString();
			int32				surfaceID	= objectSurface.GetValue( TEXT( "ID" ) ).GetInt();
			Assert( surfaceID < ST_Max );
			g_SurfaceTypeNames.push_back( std::make_pair( name, ( ESurfaceType )surfaceID ) );
		}
	}

	// Is need cook editor content (include dev content)
	g_IsCookEditorContent			= g_Config.GetValue( CT_Editor, TEXT( "Editor.CookPackages" ), TEXT( "CookEditorContent" ) ).GetBool();
	if ( g_IsCookEditorContent )
	{
		Warnf( TEXT( "Enabled cook editor content\n" ) );
	}

	// Is allow shader debug dump
	g_AllowDebugShaderDump			= g_Config.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "AllowShaderDebugDump" ) ).GetBool() || g_CommandLine.HasParam( TEXT( "-shaderdump" ) );
#endif // WITH_EDITOR
}

#include "Misc/CoreGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/Archive.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectGlobals.h"
/*
==================
CEngineLoop::LoadScriptPackage
==================
*/
void CEngineLoop::LoadScriptPackage()
{
	// Load script package
#if 0
	std::wstring		scriptFile = CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR TEXT( "Scripts.classes" ), Sys_GameDir().c_str() );
	CObjectPackage		objectPackage;
	if ( !objectPackage.Load( scriptFile ) )
	{
		Sys_Errorf( TEXT( "Failed to load script package '%s'\n" ), scriptFile.c_str() );
		return;
	}
	Logf( TEXT( "Script package '%s' loaded\n" ), scriptFile.c_str() );

	for ( uint32 index = 0, numObjects = objectPackage.GetNumObjects(); index < numObjects; ++index )
	{
		CObject*	object = objectPackage.GetObject( index );
		if ( IsA<CEnum>( object ) || IsA<CStruct>( object ) || IsA<CClass>( object ) )
		{
			object->AddToRoot();
		}
	}
#endif // 0

	std::wstring		scriptFile = CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR TEXT( "%s.classes" ), Sys_GameDir().c_str(), g_GameName.c_str() );
	CObjectPackage*		classesPkg = LoadPackage( nullptr, scriptFile.c_str() );
}

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
		std::wstring		commandletName = CString::ToUpper( paramValues[0] );
		g_IsCooker			= commandletName == CString::ToUpper( TEXT( "CookPackages" ) ) || commandletName == CString::ToUpper( TEXT( "CCookPackagesCommandlet" ) );
		g_IsScriptCompiler	= commandletName == CString::ToUpper( TEXT( "ScriptCompiler" ) ) || commandletName == CString::ToUpper( TEXT( "CScriptCompilerCommandlet" ) );
		g_IsCommandlet		= !g_IsCooker && !g_IsScriptCompiler;
	}

	g_IsGame = !g_IsEditor && !g_IsCooker && !g_IsCommandlet && !g_IsScriptCompiler;
#endif // WITH_EDITOR

	Sys_GetCookedContentPath( g_Platform, g_CookedDir );

	g_Log->Init();
	int32		result = Sys_PlatformPreInit();
	
	// Initialize the system
	CSystem::Get().Init();

	// Load script package
	if ( !g_IsScriptCompiler )
	{
		LoadScriptPackage();
	}

	// Bind all fields
	for ( TObjectIterator<CField> it; it; ++it )
	{
		it->Bind();
		CClass* theClass = Cast<CClass>( *it );
		if ( theClass && !CReflectionEnvironment::Get().FindClass( theClass->GetName().c_str() ) )
		{
			CReflectionEnvironment::Get().AddClass( theClass );
		}
	}

	// Loading table of contents
	if ( !g_IsEditor && !g_IsCooker )
	{
		std::wstring	tocPath = g_CookedDir + PATH_SEPARATOR + CTableOfContets::GetNameTOC();

#if WITH_EDITOR
		if ( !g_FileSystem->IsExistFile( tocPath ) )
		{
			CCommandLine		commandLine;
			commandLine.Init( TEXT( "-commandlet=CookerSync" ) );
			CBaseCommandlet::ExecCommandlet( commandLine );
		}
#endif // WITH_EDITOR
		
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
		Sys_Errorf( TEXT( "Cooked directory '%s' not exist. For work need cook packages" ), g_CookedDir.c_str() );
		return -1;
	}

	g_Window->Create( ANSI_TO_TCHAR( ENGINE_NAME " " ENGINE_VERSION_STRING ), 1, 1, SW_Default );
	g_ScriptEngine->Init();
	g_RHI->Init( g_IsEditor );

	Logf( TEXT( "User: %s//%s\n" ), Sys_ComputerName().c_str(), Sys_UserName().c_str() );
	Logf( TEXT( "Started with arguments: %s\n" ), InCmdLine );

	// Creating engine from config
	{
		std::wstring		classEngineName = TEXT( "CBaseEngine" );
		if ( !g_IsEditor )
		{
			classEngineName = g_Config.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "Class" ) ).GetString().c_str();
		}
#if WITH_EDITOR
		else
		{
			classEngineName = g_Config.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Class" ) ).GetString().c_str();
		}
#endif // WITH_EDITOR

		const CClass*		lclass = CReflectionEnvironment::Get().FindClass( classEngineName.c_str() );
		AssertMsg( lclass, TEXT( "Class engine %s not found" ), classEngineName.c_str() );
		
		CObjectPackage*		enginePackage = CreatePackage( nullptr, TEXT( "Engine" ) );
		g_Engine			= lclass->CreateObject<CBaseEngine>( enginePackage );
		Assert( g_Engine );
		g_Engine->AddToRoot();
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
		CConfigValue		configEditorStartupMap = g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "EditorStartupMap" ) );
		if ( configEditorStartupMap.IsA( CConfigValue::T_String ) )
		{
			map = configEditorStartupMap.GetString();
		}
	}
	else
#endif // WITH_EDITOR
	{
		// Get map for loading in game
		CConfigValue		configGameDefaultMap = g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "GameDefaultMap" ) );
		if ( configGameDefaultMap.IsA( CConfigValue::T_String ) )
		{
			map = configGameDefaultMap.GetString();
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
		Sys_SetSplashText( STT_StartupProgress, CString::Format( TEXT( "Loading map '%s'..." ), map.c_str() ).c_str() );

		std::wstring		error;
		bool				bAbsolutePath = g_FileSystem->IsAbsolutePath( map );
		bool				successed = g_Engine->LoadMap( !bAbsolutePath ? CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s" ), g_CookedDir.c_str(), map.c_str() ) : map, error );
		if ( !successed )
		{
			Sys_Errorf( TEXT( "Failed loading map '%s'. Error: %s" ), map.c_str(), error.c_str() );
			result = 2;
		}
	}
	else if ( !g_IsEditor )
	{
		Sys_Errorf( TEXT( "In game config not setted or not valid default map (parameter 'GameDefaultMap')" ) );
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
	CObjectGC::Get().CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );
	CObject::CleanupLinkerMap();
	g_Log->TearDown();
	CSystem::Get().Shutdown();
	g_Config.Shutdown();
	g_CommandLine.Shutdown();
}
