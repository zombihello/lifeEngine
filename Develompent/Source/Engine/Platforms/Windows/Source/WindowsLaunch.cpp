#include <exception>
#include <SDL.h>

#include "Core.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "WindowsGlobals.h"
#include "Misc/Guid.h"
#include "Misc/CommandLine.h"
#include "Containers/StringConv.h"
#include "EngineLoop.h"
#include "D3D11RHI.h"
#include "D3D11Viewport.h"
#include "D3D11DeviceContext.h"
#include "System/Archive.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"
#include "Logger/LoggerMacros.h"
#include "Math/Color.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Misc/Misc.h"
#include "System/Config.h"
#include "System/SplashScreen.h"

#if WITH_EDITOR
#include "WorldEd.h"
#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"
#endif // WITH_EDITOR

/*
==================
Sys_PlatformPreInit
==================
*/
int32 Sys_PlatformPreInit()
{
	if ( g_IsCommandlet || g_IsCooker || g_CommandLine.HasParam( TEXT( "console" ) ) )
	{
		static_cast< CWindowsLogger* >( g_Log )->Show( true );
	}

	// Print version SDL to logs
	{
		SDL_version		sdlVersion;
		SDL_GetVersion( &sdlVersion );
		Logf( TEXT( "SDL version: %i.%i.%i\n" ), sdlVersion.major, sdlVersion.minor, sdlVersion.patch );
	}

	return 0;
}

/*
==================
Sys_PlatformInit
==================
*/
int32 Sys_PlatformInit()
{
	g_Window->ShowCursor();
	return 0;
}

/*
==================
Sys_GetCommandLine
==================
*/
std::wstring Sys_GetCommandLine()
{
	int32			argc = 0;
	LPWSTR*			argv = CommandLineToArgvW( GetCommandLineW(), &argc );
	std::wstring	commandLine;
	
	for ( int32 index = 0; index < argc; ++index )
	{
		commandLine += argv[ index ];
		commandLine += TEXT( " " );
	}

	LocalFree( argv );
	return commandLine;
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
		g_EngineLoop->ProcessEvent( windowEvent );
	}
}

/*
==================
WinMain
==================
*/
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	try
	{
		g_WinHInstance		= hInst;
		std::wstring		commandLine = Sys_GetCommandLine();
		int32				errorLevel = 0;
		
		// Pre init engine
		if ( !g_IsRequestingExit )
		{
			errorLevel = g_EngineLoop->PreInit( commandLine.c_str() );
			Assert( errorLevel == 0 );
		}

		// Show splash screen
		if ( !g_IsRequestingExit )
		{
			if ( g_IsEditor )
			{
				Sys_ShowSplash( g_Config.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );
			}
			else if ( g_IsGame )
			{
				Sys_ShowSplash( g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "Splash" ) ).GetString().c_str() );
			}
		}

		// Init engine
		if ( !g_IsRequestingExit )
		{
			errorLevel = g_EngineLoop->Init();
			Assert( errorLevel == 0 );
			if ( g_IsEditor || g_IsGame )
			{
				g_Window->Show();
				if ( g_IsEditor )
				{
					g_Window->Maximize();
				}
			}
		}

		// Hide splash screen
		Sys_HideSplash();

		// Tick engine
		while ( !g_IsRequestingExit )
		{
			// Handling system events
			Sys_ProcessWindowEvents();

			// Tick engine
			g_EngineLoop->Tick();
		}

#if WITH_EDITOR
		// Pause if we should
		if ( g_ShouldPauseBeforeExit )
		{
			Sleep( INFINITE );
		}
#endif // WITH_EDITOR

		g_EngineLoop->Exit();
	}
	catch ( std::exception InException )
	{
		Sys_Errorf( ANSI_TO_TCHAR( InException.what() ) );
		return 1;
	}
	catch ( ... )
	{
		Sys_Errorf( TEXT( "Unknown exception" ) );
		return 1;
	}

    return 0;
}