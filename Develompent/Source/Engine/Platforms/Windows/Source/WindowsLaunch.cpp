#include <exception>
#include <SDL.h>

#if WITH_EDITOR
#include <wx/wx.h>
#endif // WITH_EDITOR

#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Containers/StringConv.h"
#include "EngineLoop.h"
#include "D3D11RHI.h"
#include "D3D11Viewport.h"
#include "D3D11DeviceContext.h"
#include "System/BaseArchive.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"
#include "Logger/LoggerMacros.h"
#include "Math/Color.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Misc/Misc.h"
#include "System/Config.h"
#include "System/SplashScreen.h"

/**
 * Pre-Initialize platform
 */
int32 appPlatformPreInit( const tchar* InCmdLine )
{
	static_cast< FWindowsLogger* >( GLog )->Show( true );

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
 * Main function
 */
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE hPreInst, LPSTR lpCmdLine, int nCmdShow )
{
	try
	{
		std::wstring		commandLine = appGetCommandLine();

		int32		errorLevel = 0;
		if ( !GIsRequestingExit )
		{
			errorLevel = GEngineLoop->PreInit( commandLine.c_str() );
			check( errorLevel == 0 );
		}

#if WITH_EDITOR
		if ( GIsEditor )
		{
			errorLevel = wxEntry( hInst, hPreInst, "", nCmdShow );
			check( errorLevel == 0 );
		}
		else
#endif // WITH_EDITOR
		{		
			if ( !GIsRequestingExit )
			{
				appShowSplash( GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "Splash" ) ).GetString().c_str() );		
				errorLevel = GEngineLoop->Init();
				check( errorLevel == 0 );		
				appHideSplash();
			}
			
			while ( !GIsRequestingExit )
			{
				// Handling system events
				SWindowEvent		windowEvent;
				while ( GWindow->PollEvent( windowEvent ) )
				{
					GEngineLoop->ProcessEvent( windowEvent );
				}

				GEngineLoop->Tick();
			}
		}

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