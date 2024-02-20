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

#include "pch_appframework.h"
#include "appframework/iwindowmgr.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>

/**
 * @ingroup appframework
 * @brief Delegate for process window event
 */
DECLARE_MULTICAST_DELEGATE( COnProcessWindowEvent, const WindowEvent& /* windowEvent */ );

/**
 * @ingroup appframework
 * @brief Delegate for process input event
 */
DECLARE_MULTICAST_DELEGATE( COnProcessInputEvent, const InputEvent& /* inputEvent */ );

/**
 * @ingroup appframework
 * @brief Table for convert SDL scancode to engine button code
 */
static SDL_Scancode		s_ScanCodeToButtonCode[] =
{
	SDL_SCANCODE_UNKNOWN,				// BUTTON_CODE_NONE
	SDL_SCANCODE_0,						// KEY_0
	SDL_SCANCODE_1,						// KEY_1
	SDL_SCANCODE_2,						// KEY_2
	SDL_SCANCODE_3,						// KEY_3
	SDL_SCANCODE_4,						// KEY_4
	SDL_SCANCODE_5,						// KEY_5
	SDL_SCANCODE_6,						// KEY_6
	SDL_SCANCODE_7,						// KEY_7
	SDL_SCANCODE_8,						// KEY_8
	SDL_SCANCODE_9,						// KEY_9

	SDL_SCANCODE_A,						// KEY_A
	SDL_SCANCODE_B,						// KEY_B
	SDL_SCANCODE_C,						// KEY_C
	SDL_SCANCODE_D,						// KEY_D
	SDL_SCANCODE_E,						// KEY_E
	SDL_SCANCODE_F,						// KEY_F
	SDL_SCANCODE_G,						// KEY_G
	SDL_SCANCODE_H,						// KEY_H
	SDL_SCANCODE_I,						// KEY_I
	SDL_SCANCODE_J,						// KEY_J
	SDL_SCANCODE_K,						// KEY_K
	SDL_SCANCODE_L,						// KEY_L
	SDL_SCANCODE_M,						// KEY_M
	SDL_SCANCODE_N,						// KEY_N
	SDL_SCANCODE_O,						// KEY_O
	SDL_SCANCODE_P,						// KEY_P
	SDL_SCANCODE_Q,						// KEY_Q
	SDL_SCANCODE_R,						// KEY_R
	SDL_SCANCODE_S,						// KEY_S
	SDL_SCANCODE_T,						// KEY_T
	SDL_SCANCODE_U,						// KEY_U
	SDL_SCANCODE_V,						// KEY_V
	SDL_SCANCODE_W,						// KEY_W
	SDL_SCANCODE_X,						// KEY_X
	SDL_SCANCODE_Y,						// KEY_Y
	SDL_SCANCODE_Z,						// KEY_Z

	SDL_SCANCODE_KP_0,					// KEYPAD_0
	SDL_SCANCODE_KP_1,					// KEYPAD_1
	SDL_SCANCODE_KP_2,					// KEYPAD_2
	SDL_SCANCODE_KP_3,					// KEYPAD_3
	SDL_SCANCODE_KP_4,					// KEYPAD_4
	SDL_SCANCODE_KP_5,					// KEYPAD_5
	SDL_SCANCODE_KP_6,					// KEYPAD_6
	SDL_SCANCODE_KP_7,					// KEYPAD_7
	SDL_SCANCODE_KP_8,					// KEYPAD_8
	SDL_SCANCODE_KP_9,					// KEYPAD_9
	SDL_SCANCODE_KP_DIVIDE,				// KEYPAD_DIVIDE
	SDL_SCANCODE_KP_MULTIPLY,			// KEYPAD_MULTIPLY
	SDL_SCANCODE_KP_MINUS,				// KEYPAD_MINUS
	SDL_SCANCODE_KP_PLUS,				// KEYPAD_PLUS
	SDL_SCANCODE_KP_ENTER,				// KEYPAD_ENTER
	SDL_SCANCODE_KP_DECIMAL,			// KEYPAD_DECIMAL

	SDL_SCANCODE_LEFTBRACKET,			// KEY_LBRACKET
	SDL_SCANCODE_RIGHTBRACKET,			// KEY_RBRACKET
	SDL_SCANCODE_SEMICOLON,				// KEY_SEMICOLON
	SDL_SCANCODE_APOSTROPHE,			// KEY_APOSTROPHE
	SDL_SCANCODE_GRAVE,					// KEY_BACKQUOTE
	SDL_SCANCODE_COMMA,					// KEY_COMMA
	SDL_SCANCODE_PERIOD,				// KEY_PERIOD
	SDL_SCANCODE_SLASH,					// KEY_SLASH
	SDL_SCANCODE_BACKSLASH,				// KEY_BACKSLASH
	SDL_SCANCODE_MINUS,					// KEY_MINUS
	SDL_SCANCODE_EQUALS,				// KEY_EQUAL
	SDL_SCANCODE_RETURN,				// KEY_ENTER
	SDL_SCANCODE_SPACE,					// KEY_SPACE
	SDL_SCANCODE_BACKSPACE,				// KEY_BACKSPACE
	SDL_SCANCODE_TAB,					// KEY_TAB
	SDL_SCANCODE_CAPSLOCK,				// KEY_CAPSLOCK
	SDL_SCANCODE_NUMLOCKCLEAR,			// KEY_NUMLOCK
	SDL_SCANCODE_ESCAPE,				// KEY_ESCAPE
	SDL_SCANCODE_SCROLLLOCK,			// KEY_SCROLLLOCK
	SDL_SCANCODE_INSERT,				// KEY_INSERT
	SDL_SCANCODE_DELETE,				// KEY_DELETE
	SDL_SCANCODE_HOME,					// KEY_HOME
	SDL_SCANCODE_END,					// KEY_END
	SDL_SCANCODE_PAGEUP,				// KEY_PAGEUP
	SDL_SCANCODE_PAGEDOWN,				// KEY_PAGEDOWN
	SDL_SCANCODE_PAUSE,					// KEY_BREAK
	SDL_SCANCODE_LSHIFT,				// KEY_LSHIFT
	SDL_SCANCODE_RSHIFT,				// KEY_RSHIFT
	SDL_SCANCODE_LALT,					// KEY_LALT
	SDL_SCANCODE_RALT,					// KEY_RALT
	SDL_SCANCODE_LCTRL,					// KEY_LCONTROL
	SDL_SCANCODE_RCTRL,					// KEY_RCONTROL
	SDL_SCANCODE_LGUI,					// KEY_LWIN
	SDL_SCANCODE_RGUI,					// KEY_RWIN
	SDL_SCANCODE_APPLICATION,			// KEY_APP
	SDL_SCANCODE_UP,					// KEY_UP
	SDL_SCANCODE_LEFT,					// KEY_LEFT
	SDL_SCANCODE_DOWN,					// KEY_DOWN
	SDL_SCANCODE_RIGHT,					// KEY_RIGHT

	SDL_SCANCODE_F1,					// KEY_F1
	SDL_SCANCODE_F2,					// KEY_F2
	SDL_SCANCODE_F3,					// KEY_F3
	SDL_SCANCODE_F4,					// KEY_F4
	SDL_SCANCODE_F5,					// KEY_F5
	SDL_SCANCODE_F6,					// KEY_F6
	SDL_SCANCODE_F7,					// KEY_F7
	SDL_SCANCODE_F8,					// KEY_F8
	SDL_SCANCODE_F9,					// KEY_F9
	SDL_SCANCODE_F10,					// KEY_F10
	SDL_SCANCODE_F11,					// KEY_F11
	SDL_SCANCODE_F12					// KEY_F12
};

/*
==================
ScanCodeToButtonCode
==================
*/
static FORCEINLINE EButtonCode ScanCodeToButtonCode( uint32 scancode )
{
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( s_ScanCodeToButtonCode[index] == ( SDL_Scancode )scancode )
		{
			return ( EButtonCode )index;
		}
	}

	return BUTTON_CODE_NONE;
}

/*
==================
ButtonCodeToScanCode
==================
*/
static FORCEINLINE uint32 ButtonCodeToScanCode( EButtonCode buttonCode )
{
	Assert( buttonCode >= KEY_FIRST && buttonCode <= KEY_LAST );
	return s_ScanCodeToButtonCode[( uint32 )buttonCode];
}

/*
==================
MouseButtonToButtonCode
==================
*/
static FORCEINLINE EButtonCode MouseButtonToButtonCode( uint8 buttonIndex )
{
	switch ( buttonIndex )
	{
	case 1:			return MOUSE_1;
	case 2:			return MOUSE_2;
	case 3:			return MOUSE_3;
	case 4:			return MOUSE_4;
	case 5:			return MOUSE_5;
	case 6:			return MOUSE_6;
	case 7:			return MOUSE_7;
	case 8:			return MOUSE_8;
	default:		return BUTTON_CODE_NONE;
	}
}

/**
 * @ingroup appframework
 * @brief SDL window manager
 */
class CSDLWindowMgr : public CBaseAppSystem<IWindowMgr>
{
public:
	/**
	 * @brief Constructor
	 */
	CSDLWindowMgr();

	/**
	 * @brief Destructor
	 */
	~CSDLWindowMgr();

	/**
	 * @brief Create window
	 *
	 * @param pTitle	Title
	 * @param width		Width
	 * @param height	Height
	 * @param flags		Combinations flags of EStyleWindow for set style of window
	 * @return Return TRUE if window is created successfully, otherwise will return FALSE
	 */
	virtual bool Create( const achar* pTitle, uint32 width, uint32 height, uint32 flags = WINDOW_STYLE_DEFAULT ) override;

	/**
	 * @brief Close window
	 */
	virtual void Close() override;

	/**
	 * @brief Show window
	 * @param bShowWindow	Is need show or hide window
	 */
	virtual void ShowWindow( bool bShowWindow = true ) override;

	/**
	 * @brief Maximize window
	 */
	virtual void Maximize() override;

	/**
	 * @brief Minimize window
	 */
	virtual void Minimize() override;

	/**
	 * @brief Process window events
	 */
	virtual void ProcessEvents() override;

	/**
	 * @brief Set title of window
	 * @param pTitle	New title
	 */
	virtual void SetTitle( const achar* pTitle ) override;

	/**
	 * @brief Set size of window
	 *
	 * @param width		Width
	 * @param height	Height
	 */
	virtual void SetSize( uint32 width, uint32 height ) override;

	/**
	 * @brief Set fullscreen mode
	 * @param bFullscreen	Enable or disable fullscreen mode
	 */
	virtual void SetFullscreen( bool bFullscreen ) override;

	/**
	 * @brief Get delegate of process window event
	 * @return Return delegate of process window event
	 */
	virtual IOnProcessWindowEvent* OnProcessWindowEvent() const override;

	/**
	 * @brief Get delegate of process input event
	 * @return Return delegate of process input event
	 */
	virtual IOnProcessInputEvent* OnProcessInputEvent() const override;

	/**
	 * @brief Is window open
	 * @return Return TRUE if window is opened, otherwise will return FALSE
	 */
	virtual bool IsOpen() const override;

	/**
	 * @brief Is enabled fullscreen mode
	 * @return Return TRUE if fullscreen mode is enabled, otherwise will return FALSE
	 */
	virtual bool IsFullscreen() const override;

	/**
	 * @brief Get size window
	 *
	 * @param width		Output window width
	 * @oaram height	Output window height
	 */
	virtual void GetSize( uint32& width, uint32& height ) const override;

	/**
	 * @brief Get window handle
	 * @return Return window handle
	 */
	virtual windowHandle_t GetHandle() const override;

	/**
	 * @brief Get ID window in the engine
	 * @return Return ID window in the engine. If window not created will return -1
	 */
	virtual uint32 GetID() const override;

	/**
	 * @brief Get SDL window handle
	 * @return Return SDL window handle
	 */
	FORCEINLINE SDL_Window* GetSDLWindow() const 
	{ 
		return pSDLWindow; 
	}

private:
	bool					bFullscreen;			/**< Is enabled fullscreen mode */

	uint32					id;						/**< Window id */
	SDL_Window*				pSDLWindow;				/**< Pointer to SDL window */
	windowHandle_t			handle;					/**< OS window handle */
	COnProcessWindowEvent	onProcessWindowEvent;	/**< Delegate for process window event */
	COnProcessInputEvent	onProcessInputEvent;	/**< Delegate for process input event */
};


/*
==================
CreateWindowMgr
==================
*/
static CSDLWindowMgr*			s_pSDLWindowMgr = nullptr;
IWindowMgr* CreateWindowMgr()
{
	if ( !s_pSDLWindowMgr )
	{
		s_pSDLWindowMgr = new CSDLWindowMgr();
	}
	return s_pSDLWindowMgr;
}

/*
==================
CSDLWindowMgr::CSDLWindowMgr
==================
*/
CSDLWindowMgr::CSDLWindowMgr()
	: bFullscreen( false )
	, id( -1 )
	, pSDLWindow( nullptr )
	, handle( nullptr )
{}

/*
==================
CSDLWindowMgr::~CSDLWindowMgr
==================
*/
CSDLWindowMgr::~CSDLWindowMgr()
{
	Close();
}

/*
==================
CSDLWindowMgr::SetTitle
==================
*/
void CSDLWindowMgr::SetTitle( const achar* pTitle )
{
	if ( pSDLWindow )
	{
		SDL_SetWindowTitle( pSDLWindow, pTitle );
	}
}

/*
==================
CSDLWindowMgr::SetSize
==================
*/
void CSDLWindowMgr::SetSize( uint32 width, uint32 height )
{
	if ( pSDLWindow )
	{
		SDL_SetWindowSize( pSDLWindow, width, height );
		SDL_SetWindowPosition( pSDLWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED );
	}
}

/*
==================
CSDLWindowMgr::SetFullscreen
==================
*/
void CSDLWindowMgr::SetFullscreen( bool bFullscreen )
{
	if ( pSDLWindow )
	{
		CSDLWindowMgr::bFullscreen = bFullscreen;
		SDL_SetWindowFullscreen( pSDLWindow, bFullscreen ? SDL_WINDOW_FULLSCREEN : 0 );
	}
}

/*
==================
CSDLWindowMgr::IsOpen
==================
*/
bool CSDLWindowMgr::IsOpen() const
{
	return !!pSDLWindow;
}

/*
==================
CSDLWindowMgr::IsFullscreen
==================
*/
bool CSDLWindowMgr::IsFullscreen() const
{
	return bFullscreen;
}

/*
==================
CSDLWindowMgr::GetSize
==================
*/
void CSDLWindowMgr::GetSize( uint32& width, uint32& height ) const
{
	if ( pSDLWindow )
	{
		SDL_GetWindowSize( pSDLWindow, ( int* )&width, ( int* )&height );
	}
}

/*
==================
CSDLWindowMgr::GetHandle
==================
*/
windowHandle_t CSDLWindowMgr::GetHandle() const
{
	return handle;
}

/*
==================
CSDLWindowMgr::Create
==================
*/
bool CSDLWindowMgr::Create( const achar* pTitle, uint32 width, uint32 height, uint32 flags /* = WINDOW_STYLE_DEFAULT */ )
{
	// Do nothing if we already create the window
	if ( pSDLWindow )
	{
		Warning( "CSDLWindowMgr::Create: Window already created" );
		return true;
	}

	// Combine flags for SDL2
	uint32		sdlFlags = 0;
	if ( flags & WINDOW_STYLE_HIDDEN )
	{
		sdlFlags = SDL_WINDOW_HIDDEN;
	}
	else
	{
		sdlFlags = SDL_WINDOW_SHOWN;
	}

	if ( flags & WINDOW_STYLE_FULLSCREEN )
	{
		bFullscreen = true;
		sdlFlags |= SDL_WINDOW_FULLSCREEN;
	}
	else
	{
		if ( flags & WINDOW_STYLE_RESIZABLE )
		{
			sdlFlags |= SDL_WINDOW_RESIZABLE;
		}

		if ( !( flags & WINDOW_STYLE_DECORATED ) )
		{
			sdlFlags |= SDL_WINDOW_BORDERLESS;
		}

		if ( flags & WINDOW_STYLE_MINIMIZED )
		{
			sdlFlags |= SDL_WINDOW_MINIMIZED;
		}

		if ( flags & WINDOW_STYLE_MAXIMIZED )
		{
			sdlFlags |= SDL_WINDOW_MAXIMIZED;
		}
	}

	// Create the window
	pSDLWindow = SDL_CreateWindow( pTitle, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, sdlFlags );
	if ( !pSDLWindow )
	{
		Sys_Error( "Failed to create window (%ix%i) with title '%s' and flags 0x%X. SDL error: %s", width, height, pTitle, flags, SDL_GetError() );
		return false;
	}
	
	// Get OS handle on window
	SDL_SysWMinfo	sdlWindowInfo;
	SDL_VERSION( &sdlWindowInfo.version );
	SDL_GetWindowWMInfo( pSDLWindow, &sdlWindowInfo );

#if PLATFORM_WINDOWS
	handle = sdlWindowInfo.info.win.window;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

	Msg( "Window created (%ix%i) with title '%s', flags 0x%X and handle 0x%p", width, height, pTitle, flags, handle );
	id = SDL_GetWindowID( pSDLWindow );
	return true;
}

/*
==================
CSDLWindowMgr::Maximize
==================
*/
void CSDLWindowMgr::Maximize()
{
	if ( pSDLWindow )
	{
		SDL_MaximizeWindow( pSDLWindow );
	}
}

/*
==================
CSDLWindowMgr::Minimize
==================
*/
void CSDLWindowMgr::Minimize()
{
	if ( pSDLWindow )
	{
		SDL_MinimizeWindow( pSDLWindow );
	}
}

/*
==================
CSDLWindowMgr::Close
==================
*/
void CSDLWindowMgr::Close()
{
	if ( pSDLWindow )
	{
		SDL_DestroyWindow( pSDLWindow );
		Msg( "Window with handle 0x%p closed", handle );

		id				= ( uint32 )-1;
		pSDLWindow		= nullptr;
		handle			= nullptr;
		bFullscreen		= false;
	}
}

/*
==================
CSDLWindowMgr::ShowWindow
==================
*/
void CSDLWindowMgr::ShowWindow( bool bShowWindow /* = true */ )
{
	if ( pSDLWindow )
	{
		if ( bShowWindow )
		{
			SDL_ShowWindow( pSDLWindow );
		}
		else
		{
			SDL_HideWindow( pSDLWindow );
		}
	}
}

/*
==================
CSDLWindowMgr::ProcessEvents
==================
*/
void CSDLWindowMgr::ProcessEvents()
{
	// Do nothing if window isn't created
	if ( !pSDLWindow )
	{
		return;
	}

	// Polls for currently pending SDL2 events
	SDL_Event	sdlEvent;
	while ( SDL_PollEvent( &sdlEvent ) )
	{
		WindowEvent		windowEvent;
		InputEvent		inputEvent;

		switch ( sdlEvent.type )
		{
			// Text input event
		case SDL_TEXTINPUT:
			inputEvent.type							= InputEvent::EVENT_TEXT_INPUT;
			inputEvent.events.textInputEvent.pText	= sdlEvent.text.text;
			break;

			// Key pressed and released button events
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if ( sdlEvent.type == SDL_KEYUP )
			{
				inputEvent.type = InputEvent::EVENT_KEY_RELEASED;
			}
			else
			{
				inputEvent.type = InputEvent::EVENT_KEY_PRESSED;
			}

			inputEvent.events.key.bAlt			= sdlEvent.key.keysym.mod & KMOD_ALT;
			inputEvent.events.key.bCapsLock		= sdlEvent.key.keysym.mod & KMOD_CAPS;
			inputEvent.events.key.bControl		= sdlEvent.key.keysym.mod & KMOD_CTRL;
			inputEvent.events.key.bNumLock		= sdlEvent.key.keysym.mod & KMOD_NUM;
			inputEvent.events.key.bShift		= sdlEvent.key.keysym.mod & KMOD_SHIFT;
			inputEvent.events.key.bSuper		= sdlEvent.key.keysym.mod & KMOD_GUI;
			inputEvent.events.key.code			= ScanCodeToButtonCode( sdlEvent.key.keysym.scancode );
			break;

			// Mouse button pressed and released events
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			if ( sdlEvent.type == SDL_MOUSEBUTTONUP )
			{
				inputEvent.type = InputEvent::EVENT_MOUSE_RELEASED;
			}
			else
			{
				inputEvent.type = InputEvent::EVENT_MOUSE_PRESSED;
			}

			inputEvent.events.mouseButton.code	= MouseButtonToButtonCode( sdlEvent.button.button );
			inputEvent.events.mouseButton.x		= sdlEvent.button.x;
			inputEvent.events.mouseButton.y		= sdlEvent.button.y;
			break;

			// Moving wheel mouse event
		case SDL_MOUSEWHEEL:
			inputEvent.type						= InputEvent::EVENT_MOUSE_WHEEL;
			inputEvent.events.mouseWheel.x		= sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.x * -1 : sdlEvent.wheel.x;
			inputEvent.events.mouseWheel.y		= sdlEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? sdlEvent.wheel.y * -1 : sdlEvent.wheel.y;
			break;

			// Mouse moving event
		case SDL_MOUSEMOTION:
			inputEvent.type							= InputEvent::EVENT_MOUSE_MOVE;
			inputEvent.events.mouseMove.x			= sdlEvent.motion.x;
			inputEvent.events.mouseMove.y			= sdlEvent.motion.y;
			inputEvent.events.mouseMove.xDirection	= sdlEvent.motion.xrel;
			inputEvent.events.mouseMove.yDirection	= sdlEvent.motion.yrel;
			break;

			// Window events
		case SDL_WINDOWEVENT:
			switch ( sdlEvent.window.event )
			{
				// Close window event
			case SDL_WINDOWEVENT_CLOSE:
				windowEvent.type						= WindowEvent::EVENT_WINDOW_CLOSE;
				windowEvent.events.windowClose.windowId = sdlEvent.window.windowID;
				break;

				// Resize window event
			case SDL_WINDOWEVENT_SIZE_CHANGED:
			case SDL_WINDOWEVENT_RESIZED:
				windowEvent.type							= WindowEvent::EVENT_WINDOW_RESIZE;
				windowEvent.events.windowResize.width		= sdlEvent.window.data1;
				windowEvent.events.windowResize.height		= sdlEvent.window.data2;
				windowEvent.events.windowResize.windowId	= sdlEvent.window.windowID;
				break;

				// Gained focus event
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				windowEvent.type								= WindowEvent::EVENT_WINDOW_FOCUS_GAINED;
				windowEvent.events.windowFocusGained.windowId	= sdlEvent.window.windowID;
				break;

				// Lost focus event
			case SDL_WINDOWEVENT_FOCUS_LOST:
				windowEvent.type							= WindowEvent::EVENT_WINDOW_FOCUS_LOST;
				windowEvent.events.windowFocusLost.windowId = sdlEvent.window.windowID;
				break;

				// Move window event
			case SDL_WINDOWEVENT_MOVED:
				windowEvent.type							= WindowEvent::EVENT_WINDOW_MOVE;
				windowEvent.events.windowMove.x				= sdlEvent.window.data1;
				windowEvent.events.windowMove.y				= sdlEvent.window.data2;
				break;

				// Unknown event
			default:
				windowEvent.type = WindowEvent::EVENT_NONE;
				break;
			}
			break;
		}

		// Broadcast process window event
		if ( windowEvent.type != WindowEvent::EVENT_NONE )
		{
			onProcessWindowEvent.Broadcast( windowEvent );
		}

		// Broadcast process input event
		if ( inputEvent.type != InputEvent::EVENT_NONE )
		{
			onProcessInputEvent.Broadcast( inputEvent );
		}
	}
}

/*
==================
CSDLWindowMgr::OnProcessWindowEvent
==================
*/
IOnProcessWindowEvent* CSDLWindowMgr::OnProcessWindowEvent() const
{
	return ( IOnProcessWindowEvent* )&onProcessWindowEvent;
}

/*
==================
CSDLWindowMgr::OnProcessInputEvent
==================
*/
IOnProcessInputEvent* CSDLWindowMgr::OnProcessInputEvent() const
{
	return ( IOnProcessInputEvent* )&onProcessInputEvent;
}

/*
==================
CSDLWindowMgr::GetID
==================
*/
uint32 CSDLWindowMgr::GetID() const
{
	return id;
}