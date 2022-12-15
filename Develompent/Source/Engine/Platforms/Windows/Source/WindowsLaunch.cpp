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

/**
 * Pre-Initialize platform
 */
int32 appPlatformPreInit()
{
	if ( GIsCommandlet || GIsCooker || GCommandLine.HasParam( TEXT( "console" ) ) )
	{
		static_cast< CWindowsLogger* >( GLog )->Show( true );
	}

	// Print version SDL to logs
	{
		SDL_version		sdlVersion;
		SDL_GetVersion( &sdlVersion );
		LE_LOG( LT_Log, LC_Init, TEXT( "SDL version: %i.%i.%i" ), sdlVersion.major, sdlVersion.minor, sdlVersion.patch );
	}

	return 0;
}

/**
 * Initialize platform
 */
int32 appPlatformInit()
{
	GWindow->ShowCursor();
	return 0;
}

/**
 * Get arguments from command line
 */
std::wstring appGetCommandLine()
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

/**
 * Process window events
 */
void appProcessWindowEvents()
{
	// Handling system events
	SWindowEvent		windowEvent;
	while ( GWindow->PollEvent( windowEvent ) )
	{
		GEngineLoop->ProcessEvent( windowEvent );
	}
}

/**
 * Main function
 */
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	try
	{
		GWinHInstance		= hInst;
		std::wstring		commandLine = appGetCommandLine();
		int32				errorLevel = 0;
		
		// Pre init engine
		if ( !GIsRequestingExit )
		{
			errorLevel = GEngineLoop->PreInit( commandLine.c_str() );
			check( errorLevel == 0 );
		}

		// Show splash screen
		if ( !GIsRequestingExit )
		{
			if ( GIsEditor )
			{
				appShowSplash( GConfig.GetValue( CT_Editor, TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );
			}
			else if ( GIsGame )
			{
				appShowSplash( GConfig.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "Splash" ) ).GetString().c_str() );
			}
		}

		// Init engine
		if ( !GIsRequestingExit )
		{
			errorLevel = GEngineLoop->Init();
			check( errorLevel == 0 );
			if ( GIsEditor || GIsGame )
			{
				GWindow->Show();
			}
		}

		// Hide splash screen
		appHideSplash();

		// Tick engine
		while ( !GIsRequestingExit )
		{
			// Handling system events
			appProcessWindowEvents();

			// Tick engine
			GEngineLoop->Tick();
		}

#if WITH_EDITOR
		// Pause if we should
		if ( GShouldPauseBeforeExit )
		{
			Sleep( INFINITE );
		}
#endif // WITH_EDITOR

		GEngineLoop->Exit();
	}
	catch ( std::exception InException )
	{
		appErrorf( ANSI_TO_TCHAR( InException.what() ) );
		return 1;
	}
	catch ( ... )
	{
		appErrorf( TEXT( "Unknown exception" ) );
		return 1;
	}

    return 0;
}