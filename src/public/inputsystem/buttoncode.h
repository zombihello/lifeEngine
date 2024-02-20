/**
 * @file
 * @addtogroup inputsystem inputsystem
 *
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

#ifndef BUTTONCODE_H
#define BUTTONCODE_H

/**
 * @ingroup inputsystem
 * @brief Enumeration of button codes
 */
enum EButtonCode
{
	BUTTON_CODE_NONE = 0,								/**< Unknown button code */

	//
	// Keyboard buttons
	//

	KEY_FIRST	= 0,									/**< First keyboard key */
	KEY_NONE	= KEY_FIRST,							/**< Unknown keyboard key */

	KEY_0,												/**< Keyboard key '0' */
	KEY_1,												/**< Keyboard key '1' */
	KEY_2,												/**< Keyboard key '2' */
	KEY_3,												/**< Keyboard key '3' */
	KEY_4,												/**< Keyboard key '4' */
	KEY_5,												/**< Keyboard key '5' */
	KEY_6,												/**< Keyboard key '6' */
	KEY_7,												/**< Keyboard key '7' */
	KEY_8,												/**< Keyboard key '8' */
	KEY_9,												/**< Keyboard key '9' */
	KEY_A,												/**< Keyboard key 'A' */
	KEY_B,												/**< Keyboard key 'B' */
	KEY_C,												/**< Keyboard key 'C' */
	KEY_D,												/**< Keyboard key 'D' */
	KEY_E,												/**< Keyboard key 'E' */
	KEY_F,												/**< Keyboard key 'F' */
	KEY_G,												/**< Keyboard key 'G' */
	KEY_H,												/**< Keyboard key 'H' */
	KEY_I,												/**< Keyboard key 'I' */
	KEY_J,												/**< Keyboard key 'J' */
	KEY_K,												/**< Keyboard key 'K' */
	KEY_L,												/**< Keyboard key 'L' */
	KEY_M,												/**< Keyboard key 'M' */
	KEY_N,												/**< Keyboard key 'N' */
	KEY_O,												/**< Keyboard key 'O' */
	KEY_P,												/**< Keyboard key 'P' */
	KEY_Q,												/**< Keyboard key 'Q' */
	KEY_R,												/**< Keyboard key 'R' */
	KEY_S,												/**< Keyboard key 'S' */
	KEY_T,												/**< Keyboard key 'T' */
	KEY_U,												/**< Keyboard key 'U' */
	KEY_V,												/**< Keyboard key 'V' */
	KEY_W,												/**< Keyboard key 'W' */
	KEY_X,												/**< Keyboard key 'X' */
	KEY_Y,												/**< Keyboard key 'Y' */
	KEY_Z,												/**< Keyboard key 'Z' */
	KEYPAD_0,											/**< Keyboard key 'Num0' */
	KEYPAD_1,											/**< Keyboard key 'Num1' */
	KEYPAD_2,											/**< Keyboard key 'Num2' */
	KEYPAD_3,											/**< Keyboard key 'Num3' */
	KEYPAD_4,											/**< Keyboard key 'Num4' */
	KEYPAD_5,											/**< Keyboard key 'Num5' */
	KEYPAD_6,											/**< Keyboard key 'Num6' */
	KEYPAD_7,											/**< Keyboard key 'Num7' */
	KEYPAD_8,											/**< Keyboard key 'Num8' */
	KEYPAD_9,											/**< Keyboard key 'Num9' */
	KEYPAD_DIVIDE,										/**< Keyboard key 'Num /' */
	KEYPAD_MULTIPLY,									/**< Keyboard key 'Num *' */
	KEYPAD_MINUS,										/**< Keyboard key 'Num -' */
	KEYPAD_PLUS,										/**< Keyboard key 'Num +' */
	KEYPAD_ENTER,										/**< Keyboard key 'Num Enter' */
	KEYPAD_DECIMAL,										/**< Keyboard key 'Num . (Del)' */
	KEY_LBRACKET,										/**< Keyboard key '[' */
	KEY_RBRACKET,										/**< Keyboard key ']' */
	KEY_SEMICOLON,										/**< Keyboard key ';' */
	KEY_APOSTROPHE,										/**< Keyboard key ''' */
	KEY_BACKQUOTE,										/**< Keyboard key '`' */
	KEY_COMMA,											/**< Keyboard key ',' */
	KEY_PERIOD,											/**< Keyboard key '.' */
	KEY_SLASH,											/**< Keyboard key '/' */
	KEY_BACKSLASH,										/**< Keyboard key '\' */
	KEY_MINUS,											/**< Keyboard key '-' */
	KEY_EQUAL,											/**< Keyboard key '=' */
	KEY_ENTER,											/**< Keyboard key 'Enter' */
	KEY_SPACE,											/**< Keyboard key 'Space' */
	KEY_BACKSPACE,										/**< Keyboard key 'Back space' */
	KEY_TAB,											/**< Keyboard key 'Tab' */
	KEY_CAPSLOCK,										/**< Keyboard key 'Caps lock' */
	KEY_NUMLOCK,										/**< Keyboard key 'Num lock' */
	KEY_ESCAPE,											/**< Keyboard key 'Escape' */
	KEY_SCROLLLOCK,										/**< Keyboard key 'Scroll lock' */
	KEY_INSERT,											/**< Keyboard key 'Insert' */
	KEY_DELETE,											/**< Keyboard key 'Delete' */
	KEY_HOME,											/**< Keyboard key 'Home' */
	KEY_END,											/**< Keyboard key 'End' */
	KEY_PAGEUP,											/**< Keyboard key 'Page up' */
	KEY_PAGEDOWN,										/**< Keyboard key 'Page down' */
	KEY_BREAK,											/**< Keyboard key 'Pause' */
	KEY_LSHIFT,											/**< Keyboard key 'Left shift' */
	KEY_RSHIFT,											/**< Keyboard key 'Right shift' */
	KEY_LALT,											/**< Keyboard key 'Left alt' */
	KEY_RALT,											/**< Keyboard key 'Right alt' */
	KEY_LCONTROL,										/**< Keyboard key 'Left control' */
	KEY_RCONTROL,										/**< Keyboard key 'Right control' */
	KEY_LWIN,											/**< Keyboard key 'Left windows' */
	KEY_RWIN,											/**< Keyboard key 'Right windows' */
	KEY_APP,											/**< Keyboard key 'App' */
	KEY_UP,												/**< Keyboard key 'Up' */
	KEY_LEFT,											/**< Keyboard key 'Left' */
	KEY_DOWN,											/**< Keyboard key 'Down' */
	KEY_RIGHT,											/**< Keyboard key 'Right' */
	KEY_F1,												/**< Keyboard key 'F1' */
	KEY_F2,												/**< Keyboard key 'F2' */
	KEY_F3,												/**< Keyboard key 'F3' */
	KEY_F4,												/**< Keyboard key 'F4' */
	KEY_F5,												/**< Keyboard key 'F5' */
	KEY_F6,												/**< Keyboard key 'F6' */
	KEY_F7,												/**< Keyboard key 'F7' */
	KEY_F8,												/**< Keyboard key 'F8' */
	KEY_F9,												/**< Keyboard key 'F9' */
	KEY_F10,											/**< Keyboard key 'F10' */
	KEY_F11,											/**< Keyboard key 'F11' */
	KEY_F12,											/**< Keyboard key 'F12' */

	KEY_LAST		= KEY_F12,							/**< Last keyboard key */
	KEY_COUNT		= KEY_LAST - KEY_FIRST + 1,			/**< Count of keyboard keys */

	//
	// Mouse buttons
	//

	MOUSE_FIRST		= KEY_LAST + 1,						/**< First mouse button */

	MOUSE_1			= MOUSE_FIRST,						/**< Mouse button 1 */
	MOUSE_2,											/**< Mouse button 2 */
	MOUSE_3,											/**< Mouse button 3 */
	MOUSE_4,											/**< Mouse button 4 */
	MOUSE_5,											/**< Mouse button 5 */
	MOUSE_6,											/**< Mouse button 6 */
	MOUSE_7,											/**< Mouse button 7 */
	MOUSE_8,											/**< Mouse button 8 */
	MOUSE_WHEELUP,										/**< Mouse wheel up */
	MOUSE_WHEELDOWN,									/**< Mouse wheel down */
	MOUSE_X,											/**< Move mouse by X */
	MOUSE_Y,											/**< Move mouse by Y */

	MOUSE_LEFT		= MOUSE_1,							/**< Mouse left button */
	MOUSE_MIDDLE	= MOUSE_2,							/**< Mouse middle button */
	MOUSE_RIGHT		= MOUSE_3,							/**< Mouse right button */
	MOUSE_LAST		= MOUSE_Y,							/**< Last mouse button */
	MOUSE_COUNT		= MOUSE_LAST - MOUSE_FIRST + 1,		/**< Count of mouse buttons */

	BUTTON_CODE_LAST	= MOUSE_LAST,							/**< Last button code */
	BUTTON_CODE_COUNT	= BUTTON_CODE_LAST - KEY_FIRST + 1,		/**< Count all button codes */
};

#endif // !BUTTONCODE_H