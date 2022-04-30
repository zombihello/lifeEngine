#include <qnamespace.h>

#include "Core.h"
#include "System/InputKeyConv.h"
#include "System/ButtonCode.h"

/**
 * @ingroup WorldEd
 * Qt key info
 */
struct FQtKeyInfo
{
	Qt::Key		key;			/**< Qt key*/
	uint32		modifiers;		/**< Modifiers of key */

	/**
	 * Overload operator '=='
	 */
	FORCEINLINE bool operator==( const FQtKeyInfo& InOther ) const
	{
		return key == InOther.key && modifiers == InOther.modifiers;
	}
};

static FQtKeyInfo		GQtKeysToButtonCode[] =
{
	{ Qt::Key_unknown,			0						},			// BC_None
	{ Qt::Key_0,				0						},			// BC_Key0
	{ Qt::Key_1,				0						},			// BC_Key1
	{ Qt::Key_2,				0						},			// BC_Key2
	{ Qt::Key_3,				0						},			// BC_Key3
	{ Qt::Key_4,				0						},			// BC_Key4
	{ Qt::Key_5,				0						},			// BC_Key5
	{ Qt::Key_6,				0						},			// BC_Key6
	{ Qt::Key_7,				0						},			// BC_Key7
	{ Qt::Key_8,				0						},			// BC_Key8
	{ Qt::Key_9,				0						},			// BC_Key9

	{ Qt::Key_A,				0						},			// BC_KeyA
	{ Qt::Key_B,				0						},			// BC_KeyB
	{ Qt::Key_C,				0						},			// BC_KeyC
	{ Qt::Key_D,				0						},			// BC_KeyD
	{ Qt::Key_E,				0						},			// BC_KeyE
	{ Qt::Key_F,				0						},			// BC_KeyF
	{ Qt::Key_G,				0						},			// BC_KeyG
	{ Qt::Key_H,				0						},			// BC_KeyH
	{ Qt::Key_I,				0						},			// BC_KeyI
	{ Qt::Key_J,				0						},			// BC_KeyJ
	{ Qt::Key_K,				0						},			// BC_KeyK
	{ Qt::Key_L,				0						},			// BC_KeyL
	{ Qt::Key_M,				0						},			// BC_KeyM
	{ Qt::Key_N,				0						},			// BC_KeyN
	{ Qt::Key_O,				0						},			// BC_KeyO
	{ Qt::Key_P,				0						},			// BC_KeyP
	{ Qt::Key_Q,				0						},			// BC_KeyQ
	{ Qt::Key_R,				0						},			// BC_KeyR
	{ Qt::Key_S,				0						},			// BC_KeyS
	{ Qt::Key_T,				0						},			// BC_KeyT
	{ Qt::Key_U,				0						},			// BC_KeyU
	{ Qt::Key_V,				0						},			// BC_KeyV
	{ Qt::Key_W,				0						},			// BC_KeyW
	{ Qt::Key_X,				0						},			// BC_KeyX
	{ Qt::Key_Y,				0						},			// BC_KeyY
	{ Qt::Key_Z,				0						},			// BC_KeyZ

	{ Qt::Key_0,				Qt::KeypadModifier		},			// BC_KeyPad0
	{ Qt::Key_1,				Qt::KeypadModifier		},			// BC_KeyPad1
	{ Qt::Key_2,				Qt::KeypadModifier		},			// BC_KeyPad2
	{ Qt::Key_3,				Qt::KeypadModifier		},			// BC_KeyPad3
	{ Qt::Key_4,				Qt::KeypadModifier		},			// BC_KeyPad4
	{ Qt::Key_5,				Qt::KeypadModifier		},			// BC_KeyPad5
	{ Qt::Key_6,				Qt::KeypadModifier		},			// BC_KeyPad6
	{ Qt::Key_7,				Qt::KeypadModifier		},			// BC_KeyPad7
	{ Qt::Key_8,				Qt::KeypadModifier		},			// BC_KeyPad8
	{ Qt::Key_9,				Qt::KeypadModifier		},			// BC_KeyPad9
	{ Qt::Key_Slash,			Qt::KeypadModifier		},			// BC_KeyPadDivide
	{ Qt::Key_Asterisk,			Qt::KeypadModifier		},			// BC_KeyPadMultiply
	{ Qt::Key_Minus,			Qt::KeypadModifier		},			// BC_KeyPadMinus
	{ Qt::Key_Plus,				Qt::KeypadModifier		},			// BC_KeyPadPlus
	{ Qt::Key_Enter,			Qt::KeypadModifier		},			// BC_KeyPadEnter
	{ Qt::Key_Period,			Qt::KeypadModifier		},			// BC_KeyPadDecimal

	{ Qt::Key_BracketLeft,		0						},			// BC_KeyLBracket
	{ Qt::Key_BracketRight,		0						},			// BC_KeyRBracket
	{ Qt::Key_Semicolon,		0						},			// BC_KeySemicolon
	{ Qt::Key_Apostrophe,		0						},			// BC_KeyApostrophe
	{ Qt::Key_AsciiTilde,		0						},			// BC_KeyBackQuote
	{ Qt::Key_Comma,			0						},			// BC_KeyComma
	{ Qt::Key_Period,			0						},			// BC_KeyPeriod
	{ Qt::Key_Slash,			0						},			// BC_KeySlash
	{ Qt::Key_Backslash,		0						},			// BC_KeyBackSlash
	{ Qt::Key_Minus,			0						},			// BC_KeyMinus
	{ Qt::Key_Equal,			0						},			// BC_KeyEqual
	{ Qt::Key_Enter,			0						},			// BC_KeyEnter
	{ Qt::Key_Space,			0						},			// BC_KeySpace
	{ Qt::Key_Backspace,		0						},			// BC_KeyBackSpace
	{ Qt::Key_Tab,				0						},			// BC_KeyTab
	{ Qt::Key_CapsLock,			0						},			// BC_KeyCapsLock
	{ Qt::Key_NumLock,			0						},			// BC_KeyNumLock
	{ Qt::Key_Escape,			0						},			// BC_KeyEscape
	{ Qt::Key_ScrollLock,		0						},			// BC_KeyScrollLock
	{ Qt::Key_Insert,			0						},			// BC_KeyInsert
	{ Qt::Key_Delete,			0						},			// BC_KeyDelete
	{ Qt::Key_Home,				0						},			// BC_KeyHome
	{ Qt::Key_End,				0						},			// BC_KeyEnd
	{ Qt::Key_PageUp,			0						},			// BC_KeyPageUp
	{ Qt::Key_PageDown,			0						},			// BC_KeyPageDown
	{ Qt::Key_Pause,			0						},			// BC_KeyBreak
	{ Qt::Key_Shift,			0						},			// BC_KeyLShift
	{ Qt::Key_Shift,			0						},			// BC_KeyRShift
	{ Qt::Key_Alt,				0						},			// BC_KeyLAlt
	{ Qt::Key_Alt,				0						},			// BC_KeyRAlt
	{ Qt::Key_Control,			0						},			// BC_KeyLControl
	{ Qt::Key_Control,			0						},			// BC_KeyRControl
	{ Qt::Key_Meta,				0						},			// BC_KeyLWin
	{ Qt::Key_Meta,				0						},			// BC_KeyRWin
	{ Qt::Key_Menu,				0						},			// BC_KeyApp
	{ Qt::Key_Up,				0						},			// BC_KeyUp
	{ Qt::Key_Left,				0						},			// BC_KeyLeft
	{ Qt::Key_Down,				0						},			// BC_KeyDown
	{ Qt::Key_Right,			0						},			// BC_KeyRight

	{ Qt::Key_F1,				0						},			// BC_KeyF1
	{ Qt::Key_F2,				0						},			// BC_KeyF2
	{ Qt::Key_F3,				0						},			// BC_KeyF3
	{ Qt::Key_F4,				0						},			// BC_KeyF4
	{ Qt::Key_F5,				0						},			// BC_KeyF5
	{ Qt::Key_F6,				0						},			// BC_KeyF6
	{ Qt::Key_F7,				0						},			// BC_KeyF7
	{ Qt::Key_F8,				0						},			// BC_KeyF8
	{ Qt::Key_F9,				0						},			// BC_KeyF9
	{ Qt::Key_F10,				0						},			// BC_KeyF10
	{ Qt::Key_F11,				0						},			// BC_KeyF11
	{ Qt::Key_F12,				0						}			// BC_KeyF12
};

EButtonCode appQtKeyToButtonCode( Qt::Key InQtKey, uint32 InModifiers )
{
	for ( uint32 index = 0; index < BC_KeyCount; ++index )
	{
		if ( GQtKeysToButtonCode[ index ] == FQtKeyInfo{ InQtKey, InModifiers } )
		{
			return ( EButtonCode )index;
		}
	}

	return BC_None;
}

EButtonCode appQtMouseButtonToButtonCode( Qt::MouseButton InButton )
{
	switch ( InButton )
	{
	case Qt::LeftButton:		return BC_Mouse1;
	case Qt::RightButton:		return BC_Mouse2;
	case Qt::MiddleButton:		return BC_Mouse3;
	case Qt::BackButton:		return BC_Mouse8;
	default:					return BC_None;
	}
}