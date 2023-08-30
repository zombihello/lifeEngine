#include <SDL.h>
#include <SDL_syswm.h>

#include <Containers/StringConv.h>
#include <Logger/BaseLogger.h>
#include <Logger/LoggerMacros.h>
#include "WindowsWindow.h"
#include "WindowsImGUI.h"

static SDL_Scancode		s_ScanCodeToButtonCode[] =
{
	SDL_SCANCODE_UNKNOWN,				// BC_None
	SDL_SCANCODE_0,						// BC_Key0
	SDL_SCANCODE_1,						// BC_Key1
	SDL_SCANCODE_2,						// BC_Key2
	SDL_SCANCODE_3,						// BC_Key3
	SDL_SCANCODE_4,						// BC_Key4
	SDL_SCANCODE_5,						// BC_Key5
	SDL_SCANCODE_6,						// BC_Key6
	SDL_SCANCODE_7,						// BC_Key7
	SDL_SCANCODE_8,						// BC_Key8
	SDL_SCANCODE_9,						// BC_Key9

	SDL_SCANCODE_A,						// BC_KeyA
	SDL_SCANCODE_B,						// BC_KeyB
	SDL_SCANCODE_C,						// BC_KeyC
	SDL_SCANCODE_D,						// BC_KeyD
	SDL_SCANCODE_E,						// BC_KeyE
	SDL_SCANCODE_F,						// BC_KeyF
	SDL_SCANCODE_G,						// BC_KeyG
	SDL_SCANCODE_H,						// BC_KeyH
	SDL_SCANCODE_I,						// BC_KeyI
	SDL_SCANCODE_J,						// BC_KeyJ
	SDL_SCANCODE_K,						// BC_KeyK
	SDL_SCANCODE_L,						// BC_KeyL
	SDL_SCANCODE_M,						// BC_KeyM
	SDL_SCANCODE_N,						// BC_KeyN
	SDL_SCANCODE_O,						// BC_KeyO
	SDL_SCANCODE_P,						// BC_KeyP
	SDL_SCANCODE_Q,						// BC_KeyQ
	SDL_SCANCODE_R,						// BC_KeyR
	SDL_SCANCODE_S,						// BC_KeyS
	SDL_SCANCODE_T,						// BC_KeyT
	SDL_SCANCODE_U,						// BC_KeyU
	SDL_SCANCODE_V,						// BC_KeyV
	SDL_SCANCODE_W,						// BC_KeyW
	SDL_SCANCODE_X,						// BC_KeyX
	SDL_SCANCODE_Y,						// BC_KeyY
	SDL_SCANCODE_Z,						// BC_KeyZ

	SDL_SCANCODE_KP_0,					// BC_KeyPad0
	SDL_SCANCODE_KP_1,					// BC_KeyPad1
	SDL_SCANCODE_KP_2,					// BC_KeyPad2
	SDL_SCANCODE_KP_3,					// BC_KeyPad3
	SDL_SCANCODE_KP_4,					// BC_KeyPad4
	SDL_SCANCODE_KP_5,					// BC_KeyPad5
	SDL_SCANCODE_KP_6,					// BC_KeyPad6
	SDL_SCANCODE_KP_7,					// BC_KeyPad7
	SDL_SCANCODE_KP_8,					// BC_KeyPad8
	SDL_SCANCODE_KP_9,					// BC_KeyPad9
	SDL_SCANCODE_KP_DIVIDE,				// BC_KeyPadDivide
	SDL_SCANCODE_KP_MULTIPLY,			// BC_KeyPadMultiply
	SDL_SCANCODE_KP_MINUS,				// BC_KeyPadMinus
	SDL_SCANCODE_KP_PLUS,				// BC_KeyPadPlus
	SDL_SCANCODE_KP_ENTER,				// BC_KeyPadEnter
	SDL_SCANCODE_KP_DECIMAL,			// BC_KeyPadDecimal

	SDL_SCANCODE_LEFTBRACKET,			// BC_KeyLBracket
	SDL_SCANCODE_RIGHTBRACKET,			// BC_KeyRBracket
	SDL_SCANCODE_SEMICOLON,				// BC_KeySemicolon
	SDL_SCANCODE_APOSTROPHE,			// BC_KeyApostrophe
	SDL_SCANCODE_GRAVE,					// BC_KeyBackQuote
	SDL_SCANCODE_COMMA,					// BC_KeyComma
	SDL_SCANCODE_PERIOD,				// BC_KeyPeriod
	SDL_SCANCODE_SLASH,					// BC_KeySlash
	SDL_SCANCODE_BACKSLASH,				// BC_KeyBackSlash
	SDL_SCANCODE_MINUS,					// BC_KeyMinus
	SDL_SCANCODE_EQUALS,				// BC_KeyEqual
	SDL_SCANCODE_RETURN,				// BC_KeyEnter
	SDL_SCANCODE_SPACE,					// BC_KeySpace
	SDL_SCANCODE_BACKSPACE,				// BC_KeyBackSpace
	SDL_SCANCODE_TAB,					// BC_KeyTab
	SDL_SCANCODE_CAPSLOCK,				// BC_KeyCapsLock
	SDL_SCANCODE_NUMLOCKCLEAR,			// BC_KeyNumLock
	SDL_SCANCODE_ESCAPE,				// BC_KeyEscape
	SDL_SCANCODE_SCROLLLOCK,			// BC_KeyScrollLock
	SDL_SCANCODE_INSERT,				// BC_KeyInsert
	SDL_SCANCODE_DELETE,				// BC_KeyDelete
	SDL_SCANCODE_HOME,					// BC_KeyHome
	SDL_SCANCODE_END,					// BC_KeyEnd
	SDL_SCANCODE_PAGEUP,				// BC_KeyPageUp
	SDL_SCANCODE_PAGEDOWN,				// BC_KeyPageDown
	SDL_SCANCODE_PAUSE,					// BC_KeyBreak
	SDL_SCANCODE_LSHIFT,				// BC_KeyLShift
	SDL_SCANCODE_RSHIFT,				// BC_KeyRShift
	SDL_SCANCODE_LALT,					// BC_KeyLAlt
	SDL_SCANCODE_RALT,					// BC_KeyRAlt
	SDL_SCANCODE_LCTRL,					// BC_KeyLControl
	SDL_SCANCODE_RCTRL,					// BC_KeyRControl
	SDL_SCANCODE_LGUI,					// BC_KeyLWin
	SDL_SCANCODE_RGUI,					// BC_KeyRWin
	SDL_SCANCODE_APPLICATION,			// BC_KeyApp
	SDL_SCANCODE_UP,					// BC_KeyUp
	SDL_SCANCODE_LEFT,					// BC_KeyLeft
	SDL_SCANCODE_DOWN,					// BC_KeyDown
	SDL_SCANCODE_RIGHT,					// BC_KeyRight

	SDL_SCANCODE_F1,					// BC_KeyF1
	SDL_SCANCODE_F2,					// BC_KeyF2
	SDL_SCANCODE_F3,					// BC_KeyF3
	SDL_SCANCODE_F4,					// BC_KeyF4
	SDL_SCANCODE_F5,					// BC_KeyF5
	SDL_SCANCODE_F6,					// BC_KeyF6
	SDL_SCANCODE_F7,					// BC_KeyF7
	SDL_SCANCODE_F8,					// BC_KeyF8
	SDL_SCANCODE_F9,					// BC_KeyF9
	SDL_SCANCODE_F10,					// BC_KeyF10
	SDL_SCANCODE_F11,					// BC_KeyF11
	SDL_SCANCODE_F12					// BC_KeyF12
};

/*
==================
Sys_ScanCodeToButtonCode
==================
*/
EButtonCode Sys_ScanCodeToButtonCode( uint32 InScancode )
{
	for ( uint32 index = 0; index < BC_KeyCount; ++index )
		if ( s_ScanCodeToButtonCode[ index ] == ( SDL_Scancode )InScancode )
		{
			return ( EButtonCode )index;
		}

	return BC_None;
}

/*
==================
Sys_ButtonCodeToScanCode
==================
*/
uint32 Sys_ButtonCodeToScanCode( EButtonCode InButtonCode )
{
	Assert( InButtonCode >= BC_KeyFirst && InButtonCode <= BC_KeyLast );
	return s_ScanCodeToButtonCode[ ( uint32 )InButtonCode ];
}

/*
==================
Sys_MouseButtonToButtonCode
==================
*/
EButtonCode Sys_MouseButtonToButtonCode( uint8 InButtonIndex )
{
	switch ( InButtonIndex )
	{
	case 1:			return BC_Mouse1;
	case 2:			return BC_Mouse2;
	case 3:			return BC_Mouse3;
	case 4:			return BC_Mouse4;
	case 5:			return BC_Mouse5;
	case 6:			return BC_Mouse6;
	case 7:			return BC_Mouse7;
	case 8:			return BC_Mouse8;
	default:		return BC_None;
	}
}

/*
==================
CWindowsWindow::CWindowsWindow
==================
*/
CWindowsWindow::CWindowsWindow() :
	isShowCursor( false ),
	isFullscreen( false ),
	id( ( uint32 )-1 ),
    sdlWindow( nullptr ),
    sdlWindowInfo( nullptr ),
    handle( nullptr )
{}

/*
==================
CWindowsWindow::~CWindowsWindow
==================
*/
CWindowsWindow::~CWindowsWindow()
{
    Close();
}

/*
==================
CWindowsWindow::ShowCursor
==================
*/
void CWindowsWindow::ShowCursor()
{
	SDL_SetRelativeMouseMode( SDL_FALSE );
	isShowCursor = true;
}

/*
==================
CWindowsWindow::HideCursor
==================
*/
void CWindowsWindow::HideCursor()
{
	SDL_SetRelativeMouseMode( SDL_TRUE );
	isShowCursor = false;
}

/*
==================
CWindowsWindow::SetTitle
==================
*/
void CWindowsWindow::SetTitle( const tchar* InTitle )
{
	Assert( sdlWindow );
	SDL_SetWindowTitle( sdlWindow, TCHAR_TO_ANSI( InTitle ) );
}

/*
==================
CWindowsWindow::SetSize
==================
*/
void CWindowsWindow::SetSize( uint32 InWidth, uint32 InHeight )
{
	Assert( sdlWindow );

	SDL_SetWindowSize( sdlWindow, InWidth, InHeight );
	SDL_SetWindowPosition( sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
}

/*
==================
CWindowsWindow::SetFullscreen
==================
*/
void CWindowsWindow::SetFullscreen( bool InIsFullscreen )
{
	Assert( sdlWindow );

	SDL_SetWindowFullscreen( sdlWindow, InIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	isFullscreen = InIsFullscreen;
}

/*
==================
CWindowsWindow::IsOpen
==================
*/
bool CWindowsWindow::IsOpen() const
{
	return sdlWindow;
}

/*
==================
CWindowsWindow::IsShowingCursor
==================
*/
bool CWindowsWindow::IsShowingCursor() const
{
	return isShowCursor;
}

/*
==================
CWindowsWindow::IsFullscreen
==================
*/
bool CWindowsWindow::IsFullscreen() const
{
	return isFullscreen;
}

/*
==================
CWindowsWindow::GetSize
==================
*/
void CWindowsWindow::GetSize( uint32& OutWidth, uint32& OutHeight ) const
{
	Assert( sdlWindow );
	SDL_GetWindowSize( sdlWindow, ( int* ) &OutWidth, ( int* ) &OutHeight );
}

/*
==================
CWindowsWindow::GetHandle
==================
*/
WindowHandle_t CWindowsWindow::GetHandle() const
{
	return handle;
}

/*
==================
CWindowsWindow::Create
==================
*/
void CWindowsWindow::Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags /* = SW_Default */ )
{
    Assert( !sdlWindow );

	// Combine flags for SDL2
    uint32              flags = 0;
	if ( InFlags & SW_Hidden )
	{
		flags = SDL_WINDOW_HIDDEN;
	}
	else
	{
		flags = SDL_WINDOW_SHOWN;
	}

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

		if ( InFlags & SW_Minimized )
		{
			flags |= SDL_WINDOW_MINIMIZED;
		}

		if ( InFlags & SW_Maximized )
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}
	}
	
	// Creating window
	sdlWindow = SDL_CreateWindow( TCHAR_TO_ANSI( InTitle ), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, InWidth, InHeight, flags );
	if ( !sdlWindow )
	{
		Sys_Errorf( TEXT( "Failed created window (%ix%i) with title '%s' and flags 0x%X. SDL error: %s" ), InWidth, InHeight, InTitle, InFlags, SDL_GetError() );
	}

	id = SDL_GetWindowID( sdlWindow );
	
	// Getting OS handle on window
	sdlWindowInfo = new SDL_SysWMinfo();

	SDL_VERSION( &sdlWindowInfo->version );
	SDL_GetWindowWMInfo( sdlWindow, sdlWindowInfo );
	handle = sdlWindowInfo->info.win.window;

	Logf( TEXT( "Window created (%ix%i) with title '%s', flags 0x%X and handle 0x%X\n" ), InWidth, InHeight, InTitle, InFlags, handle );
}

/*
==================
CWindowsWindow::Maximize
==================
*/
void CWindowsWindow::Maximize()
{
	if ( sdlWindow )
	{
		SDL_MaximizeWindow( sdlWindow );
	}
}

/*
==================
CWindowsWindow::Minimize
==================
*/
void CWindowsWindow::Minimize()
{
	if ( sdlWindow )
	{
		SDL_MinimizeWindow( sdlWindow );
	}
}

/*
==================
CWindowsWindow::Close
==================
*/
void CWindowsWindow::Close()
{
	if ( !sdlWindow )		return;

	SDL_DestroyWindow( sdlWindow );
	delete sdlWindowInfo;

	if ( !isShowCursor )
	{
		SDL_SetRelativeMouseMode( SDL_FALSE );
	}

	Logf( TEXT( "Window with handle 0x%X closed\n" ), handle );

	id = ( uint32 )-1;
	sdlWindow = nullptr;
	sdlWindowInfo = nullptr;
	handle = nullptr;
	isFullscreen = false;
	isShowCursor = false;
}

/*
==================
CWindowsWindow::Show
==================
*/
void CWindowsWindow::Show()
{
	if ( sdlWindow )
	{
		SDL_ShowWindow( sdlWindow );
	}
}

/*
==================
CWindowsWindow::Hide
==================
*/
void CWindowsWindow::Hide()
{
	if ( sdlWindow )
	{
		SDL_HideWindow( sdlWindow );
	}
}

/*
==================
CWindowsWindow::PollEvent
==================
*/
bool CWindowsWindow::PollEvent( WindowEvent& OutWindowEvent )
{
	Assert( sdlWindow );
	OutWindowEvent.type = WindowEvent::T_None;

	SDL_Event			sdlEvent;
	bool				isNotEndEvent = SDL_PollEvent( &sdlEvent );

	switch ( sdlEvent.type )
	{
		// Event of text input
	case SDL_TEXTINPUT:
		// TODO BG yehor.pohuliaka: I don't like this shit, I'm not sure about this kind of storage of the input string.
		OutWindowEvent.type = WindowEvent::T_TextInput;
		OutWindowEvent.events.textInputEvent.text = sdlEvent.text.text;
		break;

		// Event of key pressed and released buttons
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		if ( sdlEvent.type == SDL_KEYUP )
		{
			OutWindowEvent.type = WindowEvent::T_KeyReleased;
		}
		else
		{
			OutWindowEvent.type = WindowEvent::T_KeyPressed;
		}

		OutWindowEvent.events.key.isAlt			= sdlEvent.key.keysym.mod & KMOD_ALT;
		OutWindowEvent.events.key.isCapsLock	= sdlEvent.key.keysym.mod & KMOD_CAPS;
		OutWindowEvent.events.key.isControl		= sdlEvent.key.keysym.mod & KMOD_CTRL;
		OutWindowEvent.events.key.isNumLock		= sdlEvent.key.keysym.mod & KMOD_NUM;
		OutWindowEvent.events.key.isShift		= sdlEvent.key.keysym.mod & KMOD_SHIFT;
		OutWindowEvent.events.key.isSuper		= sdlEvent.key.keysym.mod & KMOD_GUI;
		OutWindowEvent.events.key.code			= Sys_ScanCodeToButtonCode( sdlEvent.key.keysym.scancode );
		break;

		// Event of mouse button pressed and released
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		if ( sdlEvent.type == SDL_MOUSEBUTTONUP )
		{
			OutWindowEvent.type = WindowEvent::T_MouseReleased;
		}
		else
		{
			OutWindowEvent.type = WindowEvent::T_MousePressed;
		}

		OutWindowEvent.events.mouseButton.code = Sys_MouseButtonToButtonCode( sdlEvent.button.button );
		OutWindowEvent.events.mouseButton.x = sdlEvent.button.x;
		OutWindowEvent.events.mouseButton.y = sdlEvent.button.y;
		break;

		// Event of moving wheel mouse
	case SDL_MOUSEWHEEL:
		OutWindowEvent.type = WindowEvent::T_MouseWheel;
		OutWindowEvent.events.mouseWheel.x = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.x * -1 : sdlEvent.wheel.x;
		OutWindowEvent.events.mouseWheel.y = sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.y * -1 : sdlEvent.wheel.y;
		break;

		// Event of mouse moving
	case SDL_MOUSEMOTION:
		OutWindowEvent.type = WindowEvent::T_MouseMove;
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
			OutWindowEvent.type = WindowEvent::T_WindowClose;
			OutWindowEvent.events.windowClose.windowId = sdlEvent.window.windowID;
			break;

			// Event of resize window
		case SDL_WINDOWEVENT_SIZE_CHANGED:
		case SDL_WINDOWEVENT_RESIZED:
			OutWindowEvent.type = WindowEvent::T_WindowResize;
			OutWindowEvent.events.windowResize.width = sdlEvent.window.data1;
			OutWindowEvent.events.windowResize.height = sdlEvent.window.data2;
			OutWindowEvent.events.windowResize.windowId = sdlEvent.window.windowID;
			break;

			// Event of gained focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			OutWindowEvent.type = WindowEvent::T_WindowFocusGained;
			OutWindowEvent.events.windowFocusGained.windowId = sdlEvent.window.windowID;

			if ( !isShowCursor )
			{
				SDL_SetRelativeMouseMode( SDL_TRUE );
			}
			break;

			// Event of lost focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			OutWindowEvent.type = WindowEvent::T_WindowFocusLost;
			OutWindowEvent.events.windowFocusLost.windowId = sdlEvent.window.windowID;

			if ( !isShowCursor )
			{
				SDL_SetRelativeMouseMode( SDL_FALSE );
			}
			break;

			// Event of move
		case SDL_WINDOWEVENT_MOVED:
			OutWindowEvent.type = WindowEvent::T_WindowMove;
			OutWindowEvent.events.windowMove.x = sdlEvent.window.data1;
			OutWindowEvent.events.windowMove.y = sdlEvent.window.data2;
			break;
		}

		break;

		// In default cast we not getting event
	default:		
		OutWindowEvent.type = WindowEvent::T_None;		
		break;
	}

	return isNotEndEvent;
}

/*
==================
CWindowsWindow::GetID
==================
*/
uint32 CWindowsWindow::GetID() const
{
	return id;
}