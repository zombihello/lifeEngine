/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BUTTONCODE_H
#define BUTTONCODE_H

#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Enumeration of button codes
 */
enum EButtonCode
{
	BC_None = 0,										/**< Unknown button code */

	//
	// Keyboard buttons
	//

	BC_KeyFirst = 0,									/**< First keyboard key */
	BC_KeyNone = BC_KeyFirst,							/**< Unknown keyboard key */

	BC_Key0,											/**< Keyboard key '0' */
	BC_Key1,											/**< Keyboard key '1' */
	BC_Key2,											/**< Keyboard key '2' */
	BC_Key3,											/**< Keyboard key '3' */
	BC_Key4,											/**< Keyboard key '4' */
	BC_Key5,											/**< Keyboard key '5' */
	BC_Key6,											/**< Keyboard key '6' */
	BC_Key7,											/**< Keyboard key '7' */
	BC_Key8,											/**< Keyboard key '8' */
	BC_Key9,											/**< Keyboard key '9' */
	BC_KeyA,											/**< Keyboard key 'A' */
	BC_KeyB,											/**< Keyboard key 'B' */
	BC_KeyC,											/**< Keyboard key 'C' */
	BC_KeyD,											/**< Keyboard key 'D' */
	BC_KeyE,											/**< Keyboard key 'E' */
	BC_KeyF,											/**< Keyboard key 'F' */
	BC_KeyG,											/**< Keyboard key 'G' */
	BC_KeyH,											/**< Keyboard key 'H' */
	BC_KeyI,											/**< Keyboard key 'I' */
	BC_KeyJ,											/**< Keyboard key 'J' */
	BC_KeyK,											/**< Keyboard key 'K' */
	BC_KeyL,											/**< Keyboard key 'L' */
	BC_KeyM,											/**< Keyboard key 'M' */
	BC_KeyN,											/**< Keyboard key 'N' */
	BC_KeyO,											/**< Keyboard key 'O' */
	BC_KeyP,											/**< Keyboard key 'P' */
	BC_KeyQ,											/**< Keyboard key 'Q' */
	BC_KeyR,											/**< Keyboard key 'R' */
	BC_KeyS,											/**< Keyboard key 'S' */
	BC_KeyT,											/**< Keyboard key 'T' */
	BC_KeyU,											/**< Keyboard key 'U' */
	BC_KeyV,											/**< Keyboard key 'V' */
	BC_KeyW,											/**< Keyboard key 'W' */
	BC_KeyX,											/**< Keyboard key 'X' */
	BC_KeyY,											/**< Keyboard key 'Y' */
	BC_KeyZ,											/**< Keyboard key 'Z' */
	BC_KeyPad0,											/**< Keyboard key 'Num0' */
	BC_KeyPad1,											/**< Keyboard key 'Num1' */
	BC_KeyPad2,											/**< Keyboard key 'Num2' */
	BC_KeyPad3,											/**< Keyboard key 'Num3' */
	BC_KeyPad4,											/**< Keyboard key 'Num4' */
	BC_KeyPad5,											/**< Keyboard key 'Num5' */
	BC_KeyPad6,											/**< Keyboard key 'Num6' */
	BC_KeyPad7,											/**< Keyboard key 'Num7' */
	BC_KeyPad8,											/**< Keyboard key 'Num8' */
	BC_KeyPad9,											/**< Keyboard key 'Num9' */
	BC_KeyPadDivide,									/**< Keyboard key 'Num /' */
	BC_KeyPadMultiply,									/**< Keyboard key 'Num *' */
	BC_KeyPadMinus,										/**< Keyboard key 'Num -' */
	BC_KeyPadPlus,										/**< Keyboard key 'Num +' */
	BC_KeyPadEnter,										/**< Keyboard key 'Num Enter' */
	BC_KeyPadDecimal,									/**< Keyboard key 'Num . (Del)' */
	BC_KeyLBracket,										/**< Keyboard key '[' */
	BC_KeyRBracket,										/**< Keyboard key ']' */
	BC_KeySemicolon,									/**< Keyboard key ';' */
	BC_KeyApostrophe,									/**< Keyboard key ''' */
	BC_KeyBackQuote,									/**< Keyboard key '`' */
	BC_KeyComma,										/**< Keyboard key ',' */
	BC_KeyPeriod,										/**< Keyboard key '.' */
	BC_KeySlash,										/**< Keyboard key '/' */
	BC_KeyBackSlash,									/**< Keyboard key '\' */
	BC_KeyMinus,										/**< Keyboard key '-' */
	BC_KeyEqual,										/**< Keyboard key '=' */
	BC_KeyEnter,										/**< Keyboard key 'Enter' */
	BC_KeySpace,										/**< Keyboard key 'Space' */
	BC_KeyBackSpace,									/**< Keyboard key 'Back space' */
	BC_KeyTab,											/**< Keyboard key 'Tab' */
	BC_KeyCapsLock,										/**< Keyboard key 'Caps lock' */
	BC_KeyNumLock,										/**< Keyboard key 'Num lock' */
	BC_KeyEscape,										/**< Keyboard key 'Escape' */
	BC_KeyScrollLock,									/**< Keyboard key 'Scroll lock' */
	BC_KeyInsert,										/**< Keyboard key 'Insert' */
	BC_KeyDelete,										/**< Keyboard key 'Delete' */
	BC_KeyHome,											/**< Keyboard key 'Home' */
	BC_KeyEnd,											/**< Keyboard key 'End' */
	BC_KeyPageUp,										/**< Keyboard key 'Page up' */
	BC_KeyPageDown,										/**< Keyboard key 'Page down' */
	BC_KeyBreak,										/**< Keyboard key 'Pause' */
	BC_KeyLShift,										/**< Keyboard key 'Left shift' */
	BC_KeyRShift,										/**< Keyboard key 'Right shift' */
	BC_KeyLAlt,											/**< Keyboard key 'Left alt' */
	BC_KeyRAlt,											/**< Keyboard key 'Right alt' */
	BC_KeyLControl,										/**< Keyboard key 'Left control' */
	BC_KeyRControl,										/**< Keyboard key 'Right control' */
	BC_KeyLWin,											/**< Keyboard key 'Left windows' */
	BC_KeyRWin,											/**< Keyboard key 'Right windows' */
	BC_KeyApp,											/**< Keyboard key 'App' */
	BC_KeyUp,											/**< Keyboard key 'Up' */
	BC_KeyLeft,											/**< Keyboard key 'Left' */
	BC_KeyDown,											/**< Keyboard key 'Down' */
	BC_KeyRight,										/**< Keyboard key 'Right' */
	BC_KeyF1,											/**< Keyboard key 'F1' */
	BC_KeyF2,											/**< Keyboard key 'F2' */
	BC_KeyF3,											/**< Keyboard key 'F3' */
	BC_KeyF4,											/**< Keyboard key 'F4' */
	BC_KeyF5,											/**< Keyboard key 'F5' */
	BC_KeyF6,											/**< Keyboard key 'F6' */
	BC_KeyF7,											/**< Keyboard key 'F7' */
	BC_KeyF8,											/**< Keyboard key 'F8' */
	BC_KeyF9,											/**< Keyboard key 'F9' */
	BC_KeyF10,											/**< Keyboard key 'F10' */
	BC_KeyF11,											/**< Keyboard key 'F11' */
	BC_KeyF12,											/**< Keyboard key 'F12' */

	BC_KeyLast = BC_KeyF12,								/**< Last keyboard key */
	BC_KeyCount = BC_KeyLast - BC_KeyFirst + 1,			/**< Count of keyboard keys */

	//
	// Mouse buttons
	//

	BC_MouseFirst = BC_KeyLast + 1,						/**< First mouse button */

	BC_Mouse1 = BC_MouseFirst,							/**< Mouse button 1 */
	BC_Mouse2,											/**< Mouse button 2 */
	BC_Mouse3,											/**< Mouse button 3 */
	BC_Mouse4,											/**< Mouse button 4 */
	BC_Mouse5,											/**< Mouse button 5 */
	BC_Mouse6,											/**< Mouse button 6 */
	BC_Mouse7,											/**< Mouse button 7 */
	BC_Mouse8,											/**< Mouse button 8 */
	BC_MouseWheelUp,									/**< Mouse wheel up */
	BC_MouseWheelDown,									/**< Mouse wheel down */
	BC_MouseX,											/**< Move mouse by X */
	BC_MouseY,											/**< Move mouse by Y */

	BC_MouseLeft = BC_Mouse1,							/**< Mouse left button */
	BC_MouseRight = BC_Mouse2,							/**< Mouse right button */
	BC_MouseMiddle = BC_Mouse3,							/**< Mouse middle button */
	BC_MouseLast = BC_MouseY,							/**< Last mouse button */
	BC_MouseCount = BC_MouseLast - BC_MouseFirst + 1,	/**< Count of mouse buttons */

	BC_Last = BC_MouseLast,								/**< Last button code */
	BC_Count = BC_Last - BC_KeyFirst + 1,				/**< Count all button codes */
};

/**
 * @ingroup Core
 * @brief Get button code by name
 * 
 * @param[in] InButtonName Button name
 * @return Button code
 * 
 * Example usage: @code appGetButtonCodeByName( TEXT( "SHIFT" ) ) @endcode
 */
EButtonCode					appGetButtonCodeByName( const tchar* InButtonName );

/**
 * @ingroup Core
 * @brief Get button name
 *
 * @param[in] InButtonCode Button code
 * @return Button name
 *
 * Example usage: @code appGetButtonName( BC_KeyA ) @endcode
 */
const tchar*				appGetButtonName( EButtonCode InButtonCode );

#endif // !BUTTONCODE_H