#include <string.h>

#include "Core.h"
#include "System/ButtonCode.h"

static const tchar* s_ButtonName[] =
{
	TEXT( "" ),							// BC_None
	TEXT( "0" ),						// BC_Key0
	TEXT( "1" ),						// BC_Key1
	TEXT( "2" ),						// BC_Key2
	TEXT( "3" ),						// BC_Key3
	TEXT( "4" ),						// BC_Key4
	TEXT( "5" ),						// BC_Key5
	TEXT( "6" ),						// BC_Key6
	TEXT( "7" ),						// BC_Key7
	TEXT( "8" ),						// BC_Key8
	TEXT( "9" ),						// BC_Key9
	TEXT( "A" ),						// BC_KeyA
	TEXT( "B" ),						// BC_KeyB
	TEXT( "C" ),						// BC_KeyC
	TEXT( "D" ),						// BC_KeyD
	TEXT( "E" ),						// BC_KeyE
	TEXT( "F" ),						// BC_KeyF
	TEXT( "G" ),						// BC_KeyG
	TEXT( "H" ),						// BC_KeyH
	TEXT( "I" ),						// BC_KeyI
	TEXT( "J" ),						// BC_KeyJ
	TEXT( "K" ),						// BC_KeyK
	TEXT( "L" ),						// BC_KeyL
	TEXT( "M" ),						// BC_KeyM
	TEXT( "N" ),						// BC_KeyN
	TEXT( "O" ),						// BC_KeyO
	TEXT( "P" ),						// BC_KeyP
	TEXT( "Q" ),						// BC_KeyQ
	TEXT( "R" ),						// BC_KeyR
	TEXT( "S" ),						// BC_KeyS
	TEXT( "T" ),						// BC_KeyT
	TEXT( "U" ),						// BC_KeyU
	TEXT( "V" ),						// BC_KeyV
	TEXT( "W" ),						// BC_KeyW
	TEXT( "X" ),						// BC_KeyX
	TEXT( "Y" ),						// BC_KeyY
	TEXT( "Z" ),						// BC_KeyZ
	TEXT( "KP0" ),						// BC_KeyPad0
	TEXT( "KP1" ),						// BC_KeyPad1
	TEXT( "KP2" ),						// BC_KeyPad2
	TEXT( "KP3" ),						// BC_KeyPad3
	TEXT( "KP4" ),						// BC_KeyPad4
	TEXT( "KP5" ),						// BC_KeyPad5
	TEXT( "KP6" ),						// BC_KeyPad6
	TEXT( "KP7" ),						// BC_KeyPad7
	TEXT( "KP8" ),						// BC_KeyPad8
	TEXT( "KP9" ),						// BC_KeyPad9
	TEXT( "KPSLASH" ),					// BC_KeyPadDivide
	TEXT( "KPMULTIPLY" ),				// BC_KeyPadMultiply
	TEXT( "KPMINUS" ),					// BC_KeyPadMinus
	TEXT( "KPPLUS" ),					// BC_KeyPadPlus
	TEXT( "KPENTER" ),					// BC_KeyPadEnter
	TEXT( "KPDEL" ),					// BC_KeyPadDecimal
	TEXT( "[" ),						// BC_KeyLBracket
	TEXT( "]" ),						// BC_KeyRBracket
	TEXT( ";" ),						// BC_KeySemicolon
	TEXT( "'" ),						// BC_KeyApostrophe
	TEXT( "`" ),						// BC_KeyBackQuote
	TEXT( "," ),						// BC_KeyComma
	TEXT( "." ),						// BC_KeyPeriod
	TEXT( "/" ),						// BC_KeySlash
	TEXT( "\\" ),						// BC_KeyBackSlash
	TEXT( "-" ),						// BC_KeyMinus
	TEXT( "=" ),						// BC_KeyEqual
	TEXT( "ENTER" ),					// BC_KeyEnter
	TEXT( "SPACE" ),					// BC_KeySpace
	TEXT( "BACKSPACE" ),				// BC_KeyBackSpace
	TEXT( "TAB" ),						// BC_KeyTab
	TEXT( "CAPSLOCK" ),					// BC_KeyCapsLock
	TEXT( "NUMLOCK" ),					// BC_KeyNumLock
	TEXT( "ESC" ),						// BC_KeyEscape
	TEXT( "SCROLLLOCK" ),				// BC_KeyScrollLock
	TEXT( "INS" ),						// BC_KeyInsert
	TEXT( "DEL" ),						// BC_KeyDelete
	TEXT( "HOME" ),						// BC_KeyHome
	TEXT( "END" ),						// BC_KeyEnd
	TEXT( "PGUP" ),						// BC_KeyPageUp
	TEXT( "PGDN" ),						// BC_KeyPageDown
	TEXT( "PAUSE" ),					// BC_KeyBreak
	TEXT( "SHIFT" ),					// BC_KeyLShift
	TEXT( "RSHIFT" ),					// BC_KeyRShift
	TEXT( "ALT" ),						// BC_KeyLAlt
	TEXT( "RALT" ),						// BC_KeyRAlt
	TEXT( "CTRL" ),						// BC_KeyLControl
	TEXT( "RCTRL" ),					// BC_KeyRControl
	TEXT( "WIN" ),						// BC_KeyLWin
	TEXT( "RWIN" ),						// BC_KeyRWin
	TEXT( "APP" ),						// BC_KeyApp
	TEXT( "UP" ),						// BC_KeyUp
	TEXT( "LEFT" ),						// BC_KeyLeft
	TEXT( "DOWN" ),						// BC_KeyDown
	TEXT( "RIGHT" ),					// BC_KeyRight
	TEXT( "F1" ),						// BC_KeyF1
	TEXT( "F2" ),						// BC_KeyF2
	TEXT( "F3" ),						// BC_KeyF3
	TEXT( "F4" ),						// BC_KeyF4
	TEXT( "F5" ),						// BC_KeyF5
	TEXT( "F6" ),						// BC_KeyF6
	TEXT( "F7" ),						// BC_KeyF7
	TEXT( "F8" ),						// BC_KeyF8
	TEXT( "F9" ),						// BC_KeyF9
	TEXT( "F10" ),						// BC_KeyF10
	TEXT( "F11" ),						// BC_KeyF11
	TEXT( "F12" ),						// BC_KeyF12

	TEXT( "MOUSE1" ),					// BC_Mouse1
	TEXT( "MOUSE2" ),					// BC_Mouse2
	TEXT( "MOUSE3" ),					// BC_Mouse3
	TEXT( "MOUSE4" ),					// BC_Mouse4
	TEXT( "MOUSE5" ),					// BC_Mouse5
	TEXT( "MOUSE6" ),					// BC_Mouse6
	TEXT( "MOUSE7" ),					// BC_Mouse7
	TEXT( "MOUSE8" ),					// BC_Mouse8
	TEXT( "MOUSEWHEELUP" ),				// BC_MouseWheelUp
	TEXT( "MOUSEWHEELDOWN" ),			// BC_MouseWheelDown
	TEXT( "MOUSEX" ),					// BC_MouseX
	TEXT( "MOUSEY" ),					// BC_MouseY				
};

/*
==================
Sys_GetButtonCodeByName
==================
*/
EButtonCode Sys_GetButtonCodeByName( const tchar* InButtonName )
{
	Assert( InButtonName );
	
	for ( uint32 index = 0; index < BC_Count; ++index )
		if ( !wcscmp( s_ButtonName[ index ], InButtonName ) )
		{
			return ( EButtonCode )index;
		}

	return BC_None;
}

/*
==================
Sys_GetButtonName
==================
*/
const tchar* Sys_GetButtonName( EButtonCode InButtonCode )
{
	if ( InButtonCode == BC_KeyCount ||
		 InButtonCode == BC_MouseCount ||
		 InButtonCode == BC_Count )
	{
		return TEXT( "" );
	}

	return s_ButtonName[ ( uint32 )InButtonCode ];
}