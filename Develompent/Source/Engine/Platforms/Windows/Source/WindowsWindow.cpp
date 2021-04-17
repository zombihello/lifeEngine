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
	isShowCursor( false ),
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
 * Show cursor
 */
void WindowsWindow::ShowCursor()
{
	SDL_SetRelativeMouseMode( SDL_FALSE );
	isShowCursor = true;
}

/**
 * Hide cursor
 */
void WindowsWindow::HideCursor()
{
	SDL_SetRelativeMouseMode( SDL_TRUE );
	isShowCursor = false;
}

/**
 * Set title of window
 */
void WindowsWindow::SetTitle( const tchar* InTitle )
{
	check( sdlWindow );
	SDL_SetWindowTitle( sdlWindow, TCHAR_TO_ANSI( InTitle ) );
}

/**
 * Set size of window
 */
void WindowsWindow::SetSize( uint32 InWidth, uint32 InHeight )
{
	check( sdlWindow );

	SDL_SetWindowSize( sdlWindow, InWidth, InHeight );
	SDL_SetWindowPosition( sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}

/**
 * Set fullscreen mode
 */
void WindowsWindow::SetFullscreen( bool InIsFullscreen )
{
	check( sdlWindow );

	SDL_SetWindowFullscreen( sdlWindow, InIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	isFullscreen = InIsFullscreen;
}

/**
 * Is window open
 */
bool WindowsWindow::IsOpen() const
{
	return sdlWindow;
}

/**
 * Is showing cursor
 */
bool WindowsWindow::IsShowingCursor() const
{
	return isShowCursor;
}

/**
 * Is enabled fullscreen mode
 */
bool WindowsWindow::IsFullscreen() const
{
	return isFullscreen;
}

/**
 * Get size window
 */
void WindowsWindow::GetSize( uint32& OutWidth, uint32& OutHeight ) const
{
	check( sdlWindow );
	SDL_GetWindowSize( sdlWindow, ( int* ) &OutWidth, ( int* ) &OutHeight );
}

/**
 * Get OS handle
 */
void* WindowsWindow::GetHandle() const
{
	return handle;
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
		appErrorf( TEXT( "Failed created window (%ix%i) with title '%s' and flags 0x%X. SDL error: %s" ), InWidth, InHeight, InTitle, InFlags, SDL_GetError() );
	}

	// Getting OS handle on window
	sdlWindowInfo = new SDL_SysWMinfo();

	SDL_VERSION( &sdlWindowInfo->version );
	SDL_GetWindowWMInfo( sdlWindow, sdlWindowInfo );
	handle = sdlWindowInfo->info.win.window;

	LE_LOG( LT_Log, LC_Init, TEXT( "Window created (%ix%i) with title '%s', flags 0x%X and handle 0x%X" ), InWidth, InHeight, InTitle, InFlags, handle );
}

/**
 * Close window
 */
void WindowsWindow::Close()
{
	check( sdlWindow );

	SDL_DestroyWindow( sdlWindow );
	delete sdlWindowInfo;

	if ( !isShowCursor )
	{
		SDL_SetRelativeMouseMode( SDL_FALSE );
	}

	LE_LOG( LT_Log, LC_General, TEXT( "Window with handle 0x%X closed" ), handle );

	sdlWindow = nullptr;
	sdlWindowInfo = nullptr;
	handle = nullptr;
	isFullscreen = false;
	isShowCursor = false;
}

/**
 * Handle window event
 */
bool WindowsWindow::PollEvent( SWindowEvent& OutWindowEvent )
{
	check( sdlWindow );

	SDL_Event			sdlEvent;
	bool				isNotEndEvent = SDL_PollEvent( &sdlEvent );

	switch ( sdlEvent.type )
	{
		// Event of text input
	case SDL_TEXTINPUT:
		// TODO BG yehor.pohuliaka: I don't like this shit, I'm not sure about this kind of storage of the input string.
		OutWindowEvent.type = SWindowEvent::T_TextInput;
		OutWindowEvent.events.textInputEvent.text = ANSI_TO_TCHAR( sdlEvent.text.text );
		break;

		// Event of key pressed and released buttons
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if ( sdlEvent.type == SDL_KEYUP )
		{
			OutWindowEvent.type = SWindowEvent::T_KeyReleased;
		}
		else
		{
			OutWindowEvent.type = SWindowEvent::T_KeyPressed;
		}

		OutWindowEvent.events.key.isAlt			= sdlEvent.key.keysym.mod & KMOD_ALT;
		OutWindowEvent.events.key.isCapsLock	= sdlEvent.key.keysym.mod & KMOD_CAPS;
		OutWindowEvent.events.key.isControl		= sdlEvent.key.keysym.mod & KMOD_CTRL;
		OutWindowEvent.events.key.isNumLock		= sdlEvent.key.keysym.mod & KMOD_NUM;
		OutWindowEvent.events.key.isShift		= sdlEvent.key.keysym.mod & KMOD_SHIFT;
		OutWindowEvent.events.key.isSuper		= sdlEvent.key.keysym.mod & KMOD_GUI;
		//OutWindowEvent.events.key.code			= ButtonCode_ScanCodeToButtonCode( sdlEvent.key.keysym.scancode );
		break;

		// Event of mouse button pressed and released
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if ( sdlEvent.type == SDL_MOUSEBUTTONUP )
		{
			OutWindowEvent.type = SWindowEvent::T_MouseReleased;
		}
		else
		{
			OutWindowEvent.type = SWindowEvent::T_MousePressed;
		}

		//OutWindowEvent.events.mouseButton.code = ButtonCode_MouseButtonToButtonCode( sdlEvent.button.button );
		OutWindowEvent.events.mouseButton.x = sdlEvent.button.x;
		OutWindowEvent.events.mouseButton.y = sdlEvent.button.y;
		break;

		// Event of moving wheel mouse
	case SDL_MOUSEWHEEL:
		OutWindowEvent.type = SWindowEvent::T_MouseWheel;
		OutWindowEvent.events.mouseWheel.x = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.x * -1 : sdlEvent.wheel.x;
		OutWindowEvent.events.mouseWheel.y = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.y * -1 : sdlEvent.wheel.y;
		break;

		// Event of mouse moving
	case SDL_MOUSEMOTION:
		OutWindowEvent.type = SWindowEvent::T_MouseMove;
		OutWindowEvent.events.mouseMove.x = sdlEvent.motion.x;
		OutWindowEvent.events.mouseMove.y = sdlEvent.motion.y;
		OutWindowEvent.events.mouseMove.xDirection = sdlEvent.motion.xrel;
		OutWindowEvent.events.mouseMove.yDirection = sdlEvent.motion.yrel;
		break;

		// Events of window
	case SDL_WINDOWEVENT:
		switch ( sdlEvent.window.event )
		{
			// Event of close window
		case SDL_WINDOWEVENT_CLOSE:
			OutWindowEvent.type = SWindowEvent::T_WindowClose;
			break;

			// Event of resize window
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESIZED:
			OutWindowEvent.type = SWindowEvent::T_WindowResize;
			OutWindowEvent.events.windowResize.width = sdlEvent.window.data1;
			OutWindowEvent.events.windowResize.height = sdlEvent.window.data2;
			break;

			// Event of gained focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			OutWindowEvent.type = SWindowEvent::T_WindowFocusGained;

			if ( !isShowCursor )
			{
				SDL_SetRelativeMouseMode( SDL_TRUE );
			}
			break;

			// Event of lost focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			OutWindowEvent.type = SWindowEvent::T_WindowFocusLost;

			if ( !isShowCursor )
			{
				SDL_SetRelativeMouseMode( SDL_FALSE );
			}
			break;
		}
		break;

		// In default cast we not getting event
	default:		
		OutWindowEvent.type = SWindowEvent::T_None;		
		break;
	}

	return isNotEndEvent;
}