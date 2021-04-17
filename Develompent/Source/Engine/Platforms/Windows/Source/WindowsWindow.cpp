#include <SDL.h>
#include <SDL_syswm.h>

#include <Containers/StringConv.h>
#include <Logger/BaseLogger.h>
#include <Logger/LoggerMacros.h>
#include "WindowsWindow.h"

/**
 * Constructor
 */
WindowsWindow::WindowsWindow() :
	isFullscreen( false ),
    sdlWindow( nullptr ),
    sdlWindowInfo( nullptr ),
    handle( nullptr )
{}

/**
 * Destructor
 */
WindowsWindow::~WindowsWindow()
{
    Close();
}

/**
 * Create window
 */
void WindowsWindow::Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags /* = SW_Default */ )
{
    check( !sdlWindow );

	// Combine flags for SDL2
    uint32              flags = SDL_WINDOW_SHOWN;
	if ( isFullscreen || InFlags & SW_Fullscreen )
	{
		flags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		if ( InFlags & SW_Resizable )
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if ( !( InFlags & SW_Decorated ) )
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}
	}

	// Creating window
	sdlWindow = SDL_CreateWindow( TCHAR_TO_ANSI( InTitle ), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, InWidth, InHeight, flags );
	if ( !sdlWindow )
	{
		appErrorf( TEXT( "Failed created window. SDL error: %s" ), SDL_GetError() );
	}

	LE_LOG( LT_Log, LC_Init, TEXT( "Window created (%ix%i) with title '%s' and flags 0x%X" ), InWidth, InHeight, InTitle, InFlags );

	// Getting OS handle on window
	sdlWindowInfo = new SDL_SysWMinfo();

	SDL_VERSION( &sdlWindowInfo->version );
	SDL_GetWindowWMInfo( sdlWindow, sdlWindowInfo );
	handle = sdlWindowInfo->info.win.window;
}

/**
 * Close window
 */
void WindowsWindow::Close()
{
	check( sdlWindow );

	SDL_DestroyWindow( sdlWindow );
	delete sdlWindowInfo;

	sdlWindow = nullptr;
	sdlWindowInfo = nullptr;
	handle = nullptr;
	isFullscreen = false;
}