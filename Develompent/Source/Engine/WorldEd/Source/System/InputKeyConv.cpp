#include <qnamespace.h>

#include "Core.h"
#include "System/InputKeyConv.h"
#include "System/ButtonCode.h"

/**
 * @ingroup WorldEd
 * Qt key info
 */
struct SQtKeyInfo
{
	Qt::Key		key;			/**< Qt key*/
	uint32		modifiers;		/**< Modifiers of key */

	/**
	 * Overload operator '=='
	 */
	FORCEINLINE bool operator==( const SQtKeyInfo& InOther ) const
	{
		return key == InOther.key && ( modifiers == InOther.modifiers || modifiers == Qt::NoModifier );
	}
};

static SQtKeyInfo		GQtKeysToButtonCode[] =
{
	{ Qt::Key_unknown,			Qt::NoModifier			},			// BC_None
	{ Qt::Key_0,				Qt::NoModifier			},			// BC_Key0
	{ Qt::Key_1,				Qt::NoModifier			},			// BC_Key1
	{ Qt::Key_2,				Qt::NoModifier			},			// BC_Key2
	{ Qt::Key_3,				Qt::NoModifier			},			// BC_Key3
	{ Qt::Key_4,				Qt::NoModifier			},			// BC_Key4
	{ Qt::Key_5,				Qt::NoModifier			},			// BC_Key5
	{ Qt::Key_6,				Qt::NoModifier			},			// BC_Key6
	{ Qt::Key_7,				Qt::NoModifier			},			// BC_Key7
	{ Qt::Key_8,				Qt::NoModifier			},			// BC_Key8
	{ Qt::Key_9,				Qt::NoModifier			},			// BC_Key9

	{ Qt::Key_A,				Qt::NoModifier			},			// BC_KeyA
	{ Qt::Key_B,				Qt::NoModifier			},			// BC_KeyB
	{ Qt::Key_C,				Qt::NoModifier			},			// BC_KeyC
	{ Qt::Key_D,				Qt::NoModifier			},			// BC_KeyD
	{ Qt::Key_E,				Qt::NoModifier			},			// BC_KeyE
	{ Qt::Key_F,				Qt::NoModifier			},			// BC_KeyF
	{ Qt::Key_G,				Qt::NoModifier			},			// BC_KeyG
	{ Qt::Key_H,				Qt::NoModifier			},			// BC_KeyH
	{ Qt::Key_I,				Qt::NoModifier			},			// BC_KeyI
	{ Qt::Key_J,				Qt::NoModifier			},			// BC_KeyJ
	{ Qt::Key_K,				Qt::NoModifier			},			// BC_KeyK
	{ Qt::Key_L,				Qt::NoModifier			},			// BC_KeyL
	{ Qt::Key_M,				Qt::NoModifier			},			// BC_KeyM
	{ Qt::Key_N,				Qt::NoModifier			},			// BC_KeyN
	{ Qt::Key_O,				Qt::NoModifier			},			// BC_KeyO
	{ Qt::Key_P,				Qt::NoModifier			},			// BC_KeyP
	{ Qt::Key_Q,				Qt::NoModifier			},			// BC_KeyQ
	{ Qt::Key_R,				Qt::NoModifier			},			// BC_KeyR
	{ Qt::Key_S,				Qt::NoModifier			},			// BC_KeyS
	{ Qt::Key_T,				Qt::NoModifier			},			// BC_KeyT
	{ Qt::Key_U,				Qt::NoModifier			},			// BC_KeyU
	{ Qt::Key_V,				Qt::NoModifier			},			// BC_KeyV
	{ Qt::Key_W,				Qt::NoModifier			},			// BC_KeyW
	{ Qt::Key_X,				Qt::NoModifier			},			// BC_KeyX
	{ Qt::Key_Y,				Qt::NoModifier			},			// BC_KeyY
	{ Qt::Key_Z,				Qt::NoModifier			},			// BC_KeyZ

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

	{ Qt::Key_BracketLeft,		Qt::NoModifier			},			// BC_KeyLBracket
	{ Qt::Key_BracketRight,		Qt::NoModifier			},			// BC_KeyRBracket
	{ Qt::Key_Semicolon,		Qt::NoModifier			},			// BC_KeySemicolon
	{ Qt::Key_Apostrophe,		Qt::NoModifier			},			// BC_KeyApostrophe
	{ Qt::Key_AsciiTilde,		Qt::NoModifier			},			// BC_KeyBackQuote
	{ Qt::Key_Comma,			Qt::NoModifier			},			// BC_KeyComma
	{ Qt::Key_Period,			Qt::NoModifier			},			// BC_KeyPeriod
	{ Qt::Key_Slash,			Qt::NoModifier			},			// BC_KeySlash
	{ Qt::Key_Backslash,		Qt::NoModifier			},			// BC_KeyBackSlash
	{ Qt::Key_Minus,			Qt::NoModifier			},			// BC_KeyMinus
	{ Qt::Key_Equal,			Qt::NoModifier			},			// BC_KeyEqual
	{ Qt::Key_Enter,			Qt::NoModifier			},			// BC_KeyEnter
	{ Qt::Key_Space,			Qt::NoModifier			},			// BC_KeySpace
	{ Qt::Key_Backspace,		Qt::NoModifier			},			// BC_KeyBackSpace
	{ Qt::Key_Tab,				Qt::NoModifier			},			// BC_KeyTab
	{ Qt::Key_CapsLock,			Qt::NoModifier			},			// BC_KeyCapsLock
	{ Qt::Key_NumLock,			Qt::NoModifier			},			// BC_KeyNumLock
	{ Qt::Key_Escape,			Qt::NoModifier			},			// BC_KeyEscape
	{ Qt::Key_ScrollLock,		Qt::NoModifier			},			// BC_KeyScrollLock
	{ Qt::Key_Insert,			Qt::NoModifier			},			// BC_KeyInsert
	{ Qt::Key_Delete,			Qt::NoModifier			},			// BC_KeyDelete
	{ Qt::Key_Home,				Qt::NoModifier			},			// BC_KeyHome
	{ Qt::Key_End,				Qt::NoModifier			},			// BC_KeyEnd
	{ Qt::Key_PageUp,			Qt::NoModifier			},			// BC_KeyPageUp
	{ Qt::Key_PageDown,			Qt::NoModifier			},			// BC_KeyPageDown
	{ Qt::Key_Pause,			Qt::NoModifier			},			// BC_KeyBreak
	{ Qt::Key_Shift,			Qt::NoModifier			},			// BC_KeyLShift
	{ Qt::Key_Shift,			Qt::NoModifier			},			// BC_KeyRShift
	{ Qt::Key_Alt,				Qt::NoModifier			},			// BC_KeyLAlt
	{ Qt::Key_Alt,				Qt::NoModifier			},			// BC_KeyRAlt
	{ Qt::Key_Control,			Qt::NoModifier			},			// BC_KeyLControl
	{ Qt::Key_Control,			Qt::NoModifier			},			// BC_KeyRControl
	{ Qt::Key_Meta,				Qt::NoModifier			},			// BC_KeyLWin
	{ Qt::Key_Meta,				Qt::NoModifier			},			// BC_KeyRWin
	{ Qt::Key_Menu,				Qt::NoModifier			},			// BC_KeyApp
	{ Qt::Key_Up,				Qt::NoModifier			},			// BC_KeyUp
	{ Qt::Key_Left,				Qt::NoModifier			},			// BC_KeyLeft
	{ Qt::Key_Down,				Qt::NoModifier			},			// BC_KeyDown
	{ Qt::Key_Right,			Qt::NoModifier			},			// BC_KeyRight

	{ Qt::Key_F1,				Qt::NoModifier			},			// BC_KeyF1
	{ Qt::Key_F2,				Qt::NoModifier			},			// BC_KeyF2
	{ Qt::Key_F3,				Qt::NoModifier			},			// BC_KeyF3
	{ Qt::Key_F4,				Qt::NoModifier			},			// BC_KeyF4
	{ Qt::Key_F5,				Qt::NoModifier			},			// BC_KeyF5
	{ Qt::Key_F6,				Qt::NoModifier			},			// BC_KeyF6
	{ Qt::Key_F7,				Qt::NoModifier			},			// BC_KeyF7
	{ Qt::Key_F8,				Qt::NoModifier			},			// BC_KeyF8
	{ Qt::Key_F9,				Qt::NoModifier			},			// BC_KeyF9
	{ Qt::Key_F10,				Qt::NoModifier			},			// BC_KeyF10
	{ Qt::Key_F11,				Qt::NoModifier			},			// BC_KeyF11
	{ Qt::Key_F12,				Qt::NoModifier			}			// BC_KeyF12
};

EButtonCode appQtKeyToButtonCode( Qt::Key InQtKey, uint32 InModifiers )
{
	for ( uint32 index = 0; index < BC_KeyCount; ++index )
	{
		if ( GQtKeysToButtonCode[ index ] == SQtKeyInfo{ InQtKey, InModifiers } )
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