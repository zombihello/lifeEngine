/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <exception>

#include "Core/Core.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/Misc.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Logger/BaseLogger.h"
#include "Core/System/Archive.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/System/BaseWindow.h"
#include "Core/System/Config.h"
#include "Core/System/ThreadingBase.h"
#include "Core/System/Package.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "Core/Misc/Class.h"
#include "Core/Math/Color.h"
#include "Core/Misc/CommandLine.h"
#include "Core/Misc/TableOfContents.h"
#include "Core/System/Name.h"
#include "Core/System/Application.h"
#include "Core/LEBuild.h"
#include "Audio/Misc/AudioGlobals.h"
#include "Audio/System/AudioEngine.h"
#include "UI/UIEngine.h"
#include "UI/Misc/UIGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Engine/Render/Shaders/ShaderManager.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/InputSystem.h"
#include "Engine/Scripts/ScriptEngine.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/System/SplashScreen.h"
#include "Engine/System/BaseEngine.h"
#include "Engine/System/FullScreenMovie.h"
#include "Launch/EngineLoop.h"

#if USE_THEORA_CODEC
#include "Engine/System/FullScreenMovieTheora.h"
#else
#include "Engine/System/FullScreenMovieFallback.h"
#endif // USE_THEORA_CODEC

#if WITH_EDITOR
#include "Editor/Misc/EditorGlobals.h"
#endif // WITH_EDITOR


// GLOBALS
static CEngineLoop*		s_EngineLoop = nullptr;


/*
==================
Sys_GetCookedContentPath
==================
*/
void Sys_GetCookedContentPath( EPlatformType InPlatform, std::wstring& OutPath )
{
	// Returns the path to the cooked data for the given platform
#if WITH_EDITOR
	if ( CApplication::Get().GetType() != AT_Cooker && !g_CommandLine.HasParam( TEXT( "usecooked" ) ) )
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
Sys_ProcessWindowEvents
==================
*/
void Sys_ProcessWindowEvents()
{
	// Handling system events
	WindowEvent		windowEvent;
	while ( g_Window->PollEvent( windowEvent ) )
	{
		s_EngineLoop->ProcessEvent( windowEvent );
	}
}


/*
==================
CEngineLoop::CEngineLoop
==================
*/
CEngineLoop::CEngineLoop( CreateEngineFn_t InCreateEngineFn )
	: isInitialize( false )
	, bIsFocus( true )
	, CreateEngine( InCreateEngineFn )
{
	Assert( !s_EngineLoop && InCreateEngineFn );
	s_EngineLoop = this;
}

/*
==================
CEngineLoop::Run
==================
*/
int32 CEngineLoop::Run( const tchar* InCmdLine )
{
	int32				errorLevel = 0;
#if WITH_EDITOR
	EApplicationType	appType = CApplication::Get().GetType();
#endif // WITH_EDITOR

	// Pre init the engine
	if ( !g_IsRequestingExit )
	{
		errorLevel = PreInit( InCmdLine );		
		if ( errorLevel != 0 )
		{
			Assert( false);
			return errorLevel;
		}
	}

	// Show splash screen
	if ( !g_IsRequestingExit )
	{
#if WITH_EDITOR	
		if ( appType == AT_Editor )
		{
			Sys_ShowSplash( g_Config.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );
		}
		else if ( appType == AT_Game )
#endif // WITH_EDITOR
		{
			Sys_ShowSplash( g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "Splash" ) ).GetString().c_str() );
		}
	}

	// Init the engine
	if ( !g_IsRequestingExit )
	{
		errorLevel = Init();
		if ( errorLevel != 0 )
		{
			Assert( false );
			return errorLevel;
		}

#if WITH_EDITOR
		if ( appType == AT_Editor || appType == AT_Game )
#endif // WITH_EDITOR
		{
			g_Window->Show();
#if WITH_EDITOR
			if ( appType == AT_Editor )
			{
				g_Window->Maximize();
			}
#endif // WITH_EDITOR
		}
	}

	// Hide splash screen
	Sys_HideSplash();

	// Tick the engine
	while ( !g_IsRequestingExit )
	{
		// Handling system events
		Sys_ProcessWindowEvents();

		// Tick engine
		Tick();
	}

#if WITH_EDITOR
	// Pause if we should
	if ( g_ShouldPauseBeforeExit )
	{
		Sleep( INFINITE );
	}
#endif // WITH_EDITOR

	Exit();
	return 0;
}

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

/*
==================
CEngineLoop::PreInit
==================
*/
int32 CEngineLoop::PreInit( const tchar* InCmdLine )
{
	g_GameThreadId = Sys_GetCurrentThreadId();
	g_CommandLine.Init( InCmdLine );
	CName::StaticInit();
	InitConfigs();
	Sys_GetCookedContentPath( g_Platform, g_CookedDir );

	g_Log->Init();
	int32				result = Sys_PlatformPreInit();
	
#if WITH_EDITOR
	EApplicationType	appType = CApplication::Get().GetType();

	// Loading table of contents
	if ( appType != AT_Editor && appType != AT_Cooker )
#endif // WITH_EDITOR
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

	if ( !g_FileSystem->IsExistFile( g_CookedDir, true )
#if WITH_EDITOR
		 && appType != AT_Editor && appType != AT_Cooker
#endif // WITH_EDITOR
		 )
	{
		Sys_Errorf( TEXT( "Cooked directory '%s' not exist. For work need cook packages" ), g_CookedDir.c_str() );
		return -1;
	}

	g_Window->Create( ANSI_TO_TCHAR( ENGINE_NAME " " ENGINE_VERSION_STRING ), 1, 1, SW_Default );
	g_ScriptEngine->Init();
	g_RHI->Init();

	Logf( TEXT( "User: %s//%s\n" ), Sys_ComputerName().c_str(), Sys_UserName().c_str() );
	Logf( TEXT( "Started with arguments: %s\n" ), InCmdLine );
	
	// Create the engine
	g_Engine = CreateEngine();
	Assert( g_Engine );
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
	EApplicationType	appType = CApplication::Get().GetType();
	Assert( appType != AT_Editor || appType == AT_Editor && g_EditorEngine );
#endif // WITH_EDITOR

	// Start render thread
	StartRenderingThread();

	// Playing startup movies (only for a game)
#if WITH_EDITOR
	if ( appType == AT_Game )
#endif // WITH_EDITOR
	{
		g_FullScreenMovie->GameThreadInitiateStartupSequence();
	}

	// Start a tool
#if WITH_EDITOR
	if ( appType == AT_Tool )
	{
		LEToolRunFn_t	ToolRunFn = CApplication::Get().GetToolRunFn();
		Assert( ToolRunFn );

		double		beginToolTime = Sys_Seconds();
		bool		bToolResult = ToolRunFn();
		double		endToolTime = Sys_Seconds();

		ELogColor	logColor = bToolResult ? LC_Green : LC_Red;
		g_Log->SetTextColor( logColor );
		Logf( TEXT( "\n" ) );
		Logf( TEXT( "----------------------\n" ) );
		Logf( TEXT( "Result: %s\n" ), bToolResult ? TEXT( "seccussed" ) : TEXT( "error" ) );
		Logf( TEXT( "\n" ) );
		Logf( TEXT( "Execution of tool took: %f seconds\n" ), endToolTime - beginToolTime );

		g_IsRequestingExit = true;
		g_ShouldPauseBeforeExit = true;
		return result;
	}
#endif // WITH_EDITOR

	// Loading map
	std::wstring		map;

#if WITH_EDITOR
	if ( appType == AT_Editor )
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
#if WITH_EDITOR
	else if ( appType != AT_Editor )
#else
	else
#endif // WITH_EDITOR
	{
		Sys_Errorf( TEXT( "In game config not setted or not valid default map (parameter 'GameDefaultMap')" ) );
		result = 1;
	}

	// Stop playing startup movies (only for game)
#if WITH_EDITOR
	if ( appType == AT_Game )
#endif // WITH_EDITOR
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
	StopRenderingThread();

	g_PackageManager->Shutdown();

	g_Engine->Shutdown();
	delete g_Engine;
	g_Engine = nullptr;

	delete g_FullScreenMovie;
	g_FullScreenMovie = nullptr;

	g_AudioEngine.Shutdown();
	g_ShaderManager->Shutdown();
	g_RHI->Destroy();

	g_Window->Close();
	g_Log->TearDown();
	g_Config.Shutdown();
	g_CommandLine.Shutdown();
}
