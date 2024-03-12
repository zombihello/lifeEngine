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

#include "pch_inputsystem.h"
#include "stdlib/convar.h"
#include "engine/icvar.h"
#include "inputsystem/inputsystem_private.h"
#include "inputsystem/iinputsystem.h"

/**
 * @ingroup inputsystem
 * @brief Cvar mouse sensitivity
 */
CConVar		mouse_sensitivity( "mouse_sensitivity", "0.5", "Mouse sensitivity" );

// Table of button names
static const achar* s_pButtonNames[] =
{
	"",							// BUTTON_CODE_NONE
	"0",						// KEY_0
	"1",						// KEY_1
	"2",						// KEY_2
	"3",						// KEY_3
	"4",						// KEY_4
	"5",						// KEY_5
	"6",						// KEY_6
	"7",						// KEY_7
	"8",						// KEY_8
	"9",						// KEY_9
	"a",						// KEY_A
	"b",						// KEY_B
	"c",						// KEY_C
	"d",						// KEY_D
	"e",						// KEY_E
	"f",						// KEY_F
	"g",						// KEY_G
	"h",						// KEY_H
	"i",						// KEY_I
	"j",						// KEY_J
	"k",						// KEY_K
	"l",						// KEY_L
	"m",						// KEY_M
	"n",						// KEY_N
	"o",						// KEY_O
	"p",						// KEY_P
	"q",						// KEY_Q
	"r",						// KEY_R
	"s",						// KEY_S
	"t",						// KEY_T
	"u",						// KEY_U
	"v",						// KEY_V
	"w",						// KEY_W
	"x",						// KEY_X
	"y",						// KEY_Y
	"z",						// KEY_Z
	"kp_0",						// KEYPAD_0
	"kp_1",						// KEYPAD_1
	"kp_2",						// KEYPAD_2
	"kp_3",						// KEYPAD_3
	"kp_4",						// KEYPAD_4
	"kp_5",						// KEYPAD_5
	"kp_6",						// KEYPAD_6
	"kp_7",						// KEYPAD_7
	"kp_8",						// KEYPAD_8
	"kp_9",						// KEYPAD_9
	"kp_slash",					// KEYPAD_DIVIDE
	"kp_multiply",				// KEYPAD_MULTIPLY
	"kp_minus",					// KEYPAD_MINUS
	"kp_plus",					// KEYPAD_PLUS
	"kp_enter",					// KEYPAD_ENTER
	"kp_del",					// KEYPAD_DECIMAL
	"[",						// KEY_LBRACKET
	"]",						// KEY_RBRACKET
	"semicolon",				// KEY_SEMICOLON
	"'",						// KEY_APOSTROPHE
	"`",						// KEY_BACKQUOTE
	",",						// KEY_COMMA
	".",						// KEY_PERIOD
	"/",						// KEY_SLASH
	"\\",						// KEY_BACKSLASH
	"-",						// KEY_MINUS
	"=",						// KEY_EQUAL
	"enter",					// KEY_ENTER
	"space",					// KEY_SPACE
	"backspace",				// KEY_BACKSPACE
	"tab",						// KEY_TAB
	"capslock",					// KEY_CAPSLOCK
	"numlock",					// KEY_NUMLOCK
	"esc",						// KEY_ESCAPE
	"scrolllock",				// KEY_SCROLLLOCK
	"ins",						// KEY_INSERT
	"del",						// KEY_DELETE
	"home",						// KEY_HOME
	"end",						// KEY_END
	"pgup",						// KEY_PAGEUP
	"pgdn",						// KEY_PAGEDOWN
	"pause",					// KEY_BREAK
	"shift",					// KEY_LSHIFT
	"rshift",					// KEY_RSHIFT
	"alt",						// KEY_LALT
	"ralt",						// KEY_RALT
	"ctrl",						// KEY_LCONTROL
	"rctrl",					// KEY_RCONTROL
	"win",						// KEY_LWIN
	"rwin",						// KEY_RWIN
	"app",						// KEY_APP
	"up",						// KEY_UP
	"left",						// KEY_LEFT
	"down",						// KEY_DOWN
	"right",					// KEY_RIGHT
	"f1",						// KEY_F1
	"f2",						// KEY_F2
	"f3",						// KEY_F3
	"f4",						// KEY_F4
	"f5",						// KEY_F5
	"f6",						// KEY_F6
	"f7",						// KEY_F7
	"f8",						// KEY_F8
	"f9",						// KEY_F9
	"f10",						// KEY_F10
	"f11",						// KEY_F11
	"f12",						// KEY_F12

	"mouse1",					// MOUSE_1
	"mouse2",					// MOUSE_2
	"mouse3",					// MOUSE_3
	"mouse4",					// MOUSE_4
	"mouse5",					// MOUSE_5
	"mouse6",					// MOUSE_6
	"mouse7",					// MOUSE_7
	"mouse8",					// MOUSE_8
	"mwheelup",					// MOUSE_WHEELUP
	"mwheeldown",				// MOUSE_WHEELDOWN
	"mousex",					// MOUSE_X
	"mousey"					// MOUSE_Y				
};

/**
 * @ingroup inputsystem
 * @brief Input system
 */
class CInputSystem : public CBaseAppSystem<IInputSystem>
{
public:
	/**
	 * @brief Constructor
	 */
	CInputSystem();

	/**
	 * @brief Destructor
	 */
	~CInputSystem();

	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Attach the input system to a window
	 * @param pWindowMgr	Window manager
	 */
	virtual void AttachToWindow( IWindowMgr* pWindowMgr ) override;

	/**
	 * @brief Detach the input system from the window
	 */
	virtual void DetachFromWindow() override;

	/**
	 * @brief Clears the input state
	 */
	virtual void ClearInputState() override;

	/**
	 * @brief Set binding a key
	 *
	 * @param button	Button code
	 * @param pCommand	Console command will be executed when the button is triggered
	 */
	virtual void SetBinding( EButtonCode button, const achar* pCommand ) override;

	/**
	 * @brief Get binding for a key
	 *
	 * @param button	Button code
	 * @return Return console command for a key. If not set returns empty string
	 */
	virtual const achar* GetBindingCommand( EButtonCode button ) const override;

	/**
	 * @brief Unbind all keys
	 */
	virtual void UnbindAll() override;

	/**
	 * @brief Execute binding command of a key
	 * @param button	Button code
	 */
	void ExecBindingCommand( EButtonCode button );

	/**
	 * @brief Set relative mouse mode
	 *
	 * While the mouse is in relative mode, the cursor is hidden, and the
	 * driver will try to report continuous motion in the current window.
	 * Only relative motion events will be delivered, the mouse position
	 * will not change.
	 *
	 * @param bEnabled	 Whether or not to enable relative mode
	 */
	virtual void SetRelativeMouseMode( bool bEnabled ) override;

	/**
	 * @brief Is key down
	 *
	 * @param key	Key button
	 * @return Return TRUE if key have event 'pressed', otherwise returns FALSE
	 */
	virtual bool IsKeyDown( EButtonCode key ) const override;

	/**
	 * @brief Is key up
	 *
	 * @param key	Key button
	 * @return Return TRUE if key have event 'released', otherwise returns FALSE
	 */
	virtual bool IsKeyUp( EButtonCode key ) const override;

	/**
	 * @brief Is mouse key down
	 *
	 * @param key	Key button
	 * @return Return TRUE if mouse key have event 'pressed', otherwise returns FALSE
	 */
	virtual bool IsMouseKeyDown( EButtonCode key ) const override;

	/**
	 * @brief Is mouse key up
	 *
	 * @param key	Key button
	 * @return Return TRUE if key have event 'released', otherwise returns FALSE
	 */
	virtual bool IsMouseKeyUp( EButtonCode key ) const override;

	/**
	 * @brief Is scrolled mouse wheel
	 *
	 * @param wheel		Side scrolled mouse wheel
	 * @return Return TRUE if wheel have event 'scrolled', otherwise returns FALSE
	 */
	virtual bool IsMouseWheel( EButtonCode wheel ) const override;

	/**
	 * @brief Is mouse moved
	 *
	 * @param mouseAxis		Axis of moved mouse
	 * @return Return TRUE if mouse moved by axis, otherwise returns FALSE
	 */
	virtual bool IsMouseMoved( EButtonCode mouseAxis ) const override;

	/**
	 * @brief Is relative mouse mode enabled
	 * @return Return TRUE if relative mouse mode enabled, otherwise FALSE
	 */
	virtual bool IsRelativeMouseMode() const override;

	/**
	 * @brief Get mouse location on screen
	 * @return Return mouse location on screen
	 */
	virtual Vector2D GetMouseLocation() const override;

	/**
	 * @brief Get mouse offset on current frame
	 * @return Return mouse offset on current frame
	 */
	virtual Vector2D GetMouseOffset() const override;

	/**
	 * @brief Get mouse offset by axis
	 *
	 * @param mouseAxis	Axis of moved mouse
	 * @return Return mouse offset by axis
	 */
	virtual float GetMouseOffset( EButtonCode mouseAxis ) const override;

	/**
	 * @brief Get mouse sensitivity
	 * @return Return mouse sensitivity
	 */
	virtual float GetMouseSensitivity() const override;

	/**
	 * @brief Get current event in button
	 *
	 * @param buttonCode	Button code
	 * @return Return current event in button
	 */
	virtual EButtonEvent GetButtonEvent( EButtonCode buttonCode ) const override;

	/**
	 * @brief Get button code by name
	 *
	 * @param pButtonName	Button name
	 * @return return button code, if button with name not found will return BC_NONE
	 */
	virtual EButtonCode GetButtonCodeByName( const achar* pButtonName ) const override;

	/**
	 * @brief Get button name
	 *
	 * @param buttonCode	Button code
	 * @return return button name, if button code not valid will return empty string
	 */
	virtual const achar* GetButtonName( EButtonCode buttonCode ) const override;

private:
	/**
	 * @brief Process window event
	 *
	 * @param pUserData		User data
	 * @param windowEvent	Window event
	 */
	static void ProcessWindowEvent( void* pUserData, const WindowEvent& windowEvent );

	/**
	 * @brief Process input event
	 *
	 * @param pUserData		User data
	 * @param inputEvent	Input event
	 */
	static void ProcessInputEvent( void* pUserData, const InputEvent& inputEvent );

	bool									bRelativeMouseMode;					/**< Is relative mouse mode enabled */
	IWindowMgr*								pWindowMgr;							/**< A window manager that was attached the input system */
	IOnProcessWindowEvent::FuncDelegate*	pWindowEventDelegate;				/**< Window event delegate */
	IOnProcessInputEvent::FuncDelegate*		pInputEventDelegate;				/**< Input event delegate */
	EButtonEvent							buttonEvents[BUTTON_CODE_COUNT];	/**< Events button */
	Vector2D								mouseLocation;						/**< Location mouse on screen */
	Vector2D								mouseOffset;						/**< Offset mouse on current frame */
	std::string								binds[BUTTON_CODE_COUNT];			/**< Button binds */
};

// Input system singleton
static CInputSystem			s_InputSystem;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CInputSystem, IInputSystem, INPUTSYSTEM_INTERFACE_VERSION, s_InputSystem );


/*
==================
CInputSystem::CInputSystem
==================
*/
CInputSystem::CInputSystem() 
	: bRelativeMouseMode( false )
	, pWindowMgr( nullptr )
	, pWindowEventDelegate( nullptr )
	, pInputEventDelegate( nullptr )
	, mouseLocation( Math::vectorZero )
	, mouseOffset( Math::vectorZero )
{
	Mem_Memset( &buttonEvents, BUTTON_EVENT_NONE, sizeof( EButtonEvent ) * BUTTON_CODE_COUNT );
}

/*
==================
CInputSystem::~CInputSystem
==================
*/
CInputSystem::~CInputSystem()
{
	DetachFromWindow();
}

/*
==================
CInputSystem::Connect
==================
*/
bool CInputSystem::Connect( CreateInterfaceFn_t pFactory )
{
	return ConnectStdLib( pFactory );
}

/*
==================
CInputSystem::Disconnect
==================
*/
void CInputSystem::Disconnect()
{
	DetachFromWindow();
	DisconnectStdLib();
}

/*
==================
CInputSystem::Init
==================
*/
bool CInputSystem::Init()
{
	ConVar_Register();
	return true;
}

/*
==================
CInputSystem::Shutdown
==================
*/
void CInputSystem::Shutdown()
{
	ConVar_Unregister();
}

/*
==================
CInputSystem::AttachToWindow
==================
*/
void CInputSystem::AttachToWindow( IWindowMgr* pWindowMgr )
{
	if ( pWindowMgr )
	{
		DetachFromWindow();
	}

	pWindowEventDelegate		= pWindowMgr->OnProcessWindowEvent()->AddFunc( &CInputSystem::ProcessWindowEvent, this );
	pInputEventDelegate			= pWindowMgr->OnProcessInputEvent()->AddFunc( &CInputSystem::ProcessInputEvent, this );
	CInputSystem::pWindowMgr	= pWindowMgr;
}

/*
==================
CInputSystem::DetachFromWindow
==================
*/
void CInputSystem::DetachFromWindow()
{
	if ( pWindowMgr )
	{
		pWindowMgr->OnProcessWindowEvent()->RemoveFunc( pWindowEventDelegate );
		pWindowMgr->OnProcessInputEvent()->RemoveFunc( pInputEventDelegate );
		pWindowMgr				= nullptr;
		pWindowEventDelegate	= nullptr;
		pInputEventDelegate		= nullptr;
	}
}

/*
==================
CInputSystem::ProcessWindowEvent
==================
*/
void CInputSystem::ProcessWindowEvent( void* pUserData, const WindowEvent& windowEvent )
{
	Assert( pUserData );
	CInputSystem*	pInputSystem = ( CInputSystem* )pUserData;

	switch ( windowEvent.type )
	{
	case WindowEvent::EVENT_WINDOW_FOCUS_GAINED:
		if ( pInputSystem->bRelativeMouseMode )
		{
			Sys_SetRelativeMouseMode( true );
		}
		break;

	case WindowEvent::EVENT_WINDOW_FOCUS_LOST:
		if ( pInputSystem->bRelativeMouseMode )
		{
			Sys_SetRelativeMouseMode( false );
		}
		break;
	}
}

/*
==================
CInputSystem::ProcessInputEvent
==================
*/
void CInputSystem::ProcessInputEvent( void* pUserData, const InputEvent& inputEvent )
{
	Assert( pUserData );
	CInputSystem*		pInputSystem = ( CInputSystem* )pUserData;

	switch ( inputEvent.type )
	{
		// Key pressed
	case InputEvent::EVENT_KEY_PRESSED:			
		pInputSystem->buttonEvents[inputEvent.events.key.code] = BUTTON_EVENT_PRESSED;
		pInputSystem->ExecBindingCommand( inputEvent.events.key.code );
		break;

		// Key released
	case InputEvent::EVENT_KEY_RELEASED:
		pInputSystem->buttonEvents[inputEvent.events.key.code] = BUTTON_EVENT_RELEASED;
		pInputSystem->ExecBindingCommand( inputEvent.events.key.code );
		break;

		// Mouse pressed
	case InputEvent::EVENT_MOUSE_PRESSED:
		pInputSystem->buttonEvents[inputEvent.events.mouseButton.code] = BUTTON_EVENT_PRESSED;
		pInputSystem->ExecBindingCommand( inputEvent.events.mouseButton.code );
		break;

		// Mouse released
	case InputEvent::EVENT_MOUSE_RELEASED:
		pInputSystem->buttonEvents[inputEvent.events.mouseButton.code] = BUTTON_EVENT_RELEASED;
		pInputSystem->ExecBindingCommand( inputEvent.events.mouseButton.code );
		break;

		// Mouse move
	case InputEvent::EVENT_MOUSE_MOVE:
		pInputSystem->mouseOffset.x		+= ( float )inputEvent.events.mouseMove.xDirection;
		pInputSystem->mouseOffset.y		+= ( float )inputEvent.events.mouseMove.yDirection;
		pInputSystem->mouseLocation.x	= ( float )inputEvent.events.mouseMove.x;
		pInputSystem->mouseLocation.y	= ( float )inputEvent.events.mouseMove.y;

		if ( pInputSystem->mouseOffset.x != 0.f )
		{
			pInputSystem->buttonEvents[MOUSE_X] = BUTTON_EVENT_MOVED;
			pInputSystem->ExecBindingCommand( MOUSE_X );
		}

		if ( pInputSystem->mouseOffset.y != 0.f )
		{
			pInputSystem->buttonEvents[MOUSE_Y] = BUTTON_EVENT_MOVED;
			pInputSystem->ExecBindingCommand( MOUSE_Y );
		}
		break;

		// Mouse wheel move
	case InputEvent::EVENT_MOUSE_WHEEL:
	{
		EButtonCode		button = inputEvent.events.mouseWheel.y > 0 ? MOUSE_WHEELUP : MOUSE_WHEELDOWN;
		pInputSystem->buttonEvents[button] = BUTTON_EVENT_SCROLLED;
		pInputSystem->ExecBindingCommand( button );
		break;
	}

		// Text input
	case InputEvent::EVENT_TEXT_INPUT:
		break;
	}
}

/*
==================
CInputSystem::ClearInputState
==================
*/
void CInputSystem::ClearInputState()
{
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( buttonEvents[index] == BUTTON_EVENT_RELEASED || buttonEvents[index] == BUTTON_EVENT_SCROLLED || buttonEvents[index] == BUTTON_EVENT_MOVED )
		{
			buttonEvents[index] = BUTTON_EVENT_NONE;
		}
	}

	mouseOffset = Math::vectorZero;
}

/*
==================
CInputSystem::SetBinding
==================
*/
void CInputSystem::SetBinding( EButtonCode button, const achar* pCommand )
{
	Assert( button < BUTTON_CODE_COUNT );
	binds[button] = pCommand;
}

/*
==================
CInputSystem::UnbindAll
==================
*/
void CInputSystem::UnbindAll()
{
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		binds[index].clear();
	}
}

/*
==================
CInputSystem::GetBindingCommand
==================
*/
const achar* CInputSystem::GetBindingCommand( EButtonCode button ) const
{
	Assert( button < BUTTON_CODE_COUNT );
	return binds[button].c_str();
}

/*
==================
CInputSystem::ExecCommand
==================
*/
void CInputSystem::ExecBindingCommand( EButtonCode button )
{
	Assert( button < BUTTON_CODE_COUNT );
	if ( !binds[button].empty() )
	{
		g_pCvar->Exec( binds[button].c_str() );
	}
}

/*
==================
CInputSystem::SetRelativeMouseMode
==================
*/
void CInputSystem::SetRelativeMouseMode( bool bEnabled )
{
	bRelativeMouseMode = bEnabled;
	Sys_SetRelativeMouseMode( bEnabled );
}

/*
==================
CInputSystem::IsKeyDown
==================
*/
bool CInputSystem::IsKeyDown( EButtonCode key ) const
{
	if ( key < KEY_FIRST || key > KEY_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_PRESSED;
}

/*
==================
CInputSystem::IsKeyUp
==================
*/
bool CInputSystem::IsKeyUp( EButtonCode key ) const
{
	if ( key < KEY_FIRST || key > KEY_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_RELEASED;
}

/*
==================
CInputSystem::IsMouseKeyDown
==================
*/
bool CInputSystem::IsMouseKeyDown( EButtonCode key ) const
{
	if ( key < MOUSE_FIRST || key > MOUSE_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_PRESSED;
}

/*
==================
CInputSystem::IsMouseKeyUp
==================
*/
bool CInputSystem::IsMouseKeyUp( EButtonCode key ) const
{
	if ( key < MOUSE_FIRST || key > MOUSE_LAST )
	{
		return false;
	}

	return buttonEvents[key] == BUTTON_EVENT_RELEASED;
}

/*
==================
CInputSystem::IsMouseWheel
==================
*/
bool CInputSystem::IsMouseWheel( EButtonCode wheel ) const
{
	if ( wheel != MOUSE_WHEELUP || wheel != MOUSE_WHEELDOWN )
	{
		return false;
	}

	return buttonEvents[wheel] == BUTTON_EVENT_SCROLLED;
}

/*
==================
CInputSystem::IsMouseMoved
==================
*/
bool CInputSystem::IsMouseMoved( EButtonCode mouseAxis ) const
{
	if ( mouseAxis != MOUSE_X || mouseAxis != MOUSE_Y )
	{
		return false;
	}

	return buttonEvents[mouseAxis] == BUTTON_EVENT_MOVED;
}

/*
==================
CInputSystem::IsRelativeMouseMode
==================
*/
bool CInputSystem::IsRelativeMouseMode() const
{
	return bRelativeMouseMode;
}

/*
==================
CInputSystem::GetMouseLocation
==================
*/
Vector2D CInputSystem::GetMouseLocation() const
{
	return mouseLocation;
}

/*
==================
CInputSystem::GetMouseOffset
==================
*/
Vector2D CInputSystem::GetMouseOffset() const
{
	return mouseOffset;
}

/*
==================
CInputSystem::GetMouseOffset
==================
*/
float CInputSystem::GetMouseOffset( EButtonCode mouseAxis ) const
{
	switch ( mouseAxis )
	{
	case MOUSE_X:		return mouseOffset.x;
	case MOUSE_Y:		return mouseOffset.y;
	default:			return 0.f;
	}
}

/*
==================
CInputSystem::GetMouseSensitivity
==================
*/
float CInputSystem::GetMouseSensitivity() const
{
	return mouse_sensitivity.GetFloat();
}

/*
==================
CInputSystem::GetButtonEvent
==================
*/
EButtonEvent CInputSystem::GetButtonEvent( EButtonCode buttonCode ) const
{
	return buttonEvents[buttonCode];
}

/*
==================
CInputSystem::GetButtonCodeByName
==================
*/
EButtonCode CInputSystem::GetButtonCodeByName( const achar* pButtonName ) const
{
	Assert( pButtonName );
	for ( uint32 index = 0; index < BUTTON_CODE_COUNT; ++index )
	{
		if ( !L_Stricmp( s_pButtonNames[index], pButtonName ) )
		{
			return ( EButtonCode )index;
		}
	}

	return BUTTON_CODE_NONE;
}

/*
==================
CInputSystem::GetButtonName
==================
*/
const achar* CInputSystem::GetButtonName( EButtonCode buttonCode ) const
{
	if ( buttonCode == KEY_COUNT || buttonCode == MOUSE_COUNT || buttonCode == BUTTON_CODE_COUNT )
	{
		return "";
	}

	return s_pButtonNames[( uint32 )buttonCode];
}


/*
==================
Bind command
==================
*/
CON_COMMAND( bind, "Bind a key", FCVAR_NONE )
{
	if ( argc < 2 || !argv )
	{
		Msg( "bind <key> <command> : Attach a command to a key" );
		return;
	}

	// Get button code by name
	EButtonCode		buttonCode = s_InputSystem.GetButtonCodeByName( argv[0] );
	
	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "bind: \"%s\" isn't a valid key", argv[0] );
		return;
	}

	// Set binding
	s_InputSystem.SetBinding( buttonCode, argv[1] );
	Msg( "bind: \"%s\" = \"%s\"", argv[0], argv[1] );
}

/*
==================
Unbind command
==================
*/
CON_COMMAND( unbind, "Unbind a key", FCVAR_NONE )
{
	if ( argc < 1 || !argv )
	{
		Msg( "unbind <key> : Remove commands from a key" );
		return;
	}

	// Get button code by name
	EButtonCode		buttonCode = s_InputSystem.GetButtonCodeByName( argv[0] );

	// Do nothing if button isn't valid
	if ( buttonCode == BUTTON_CODE_NONE )
	{
		Warning( "unbind: \"%s\" isn't a valid key", argv[0] );
		return;
	}

	// Unbind a key
	s_InputSystem.SetBinding( buttonCode, "" );
	Msg( "unbind: \"%s\" is unbind", argv[0] );
}

/*
==================
UnbindAll command
==================
*/
CON_COMMAND( unbindall, "Unbind all keys", FCVAR_NONE )
{
	s_InputSystem.UnbindAll();
	Msg( "unbindall: All keys has been unbind" );
}