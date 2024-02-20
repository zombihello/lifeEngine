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

#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include "core/types.h"
#include "inputsystem/buttoncode.h"

/**
 * @ingroup inputsystem
 * @brief Struct for storage input event
 */
struct InputEvent
{
	/**
	 * @brief Enumeration of types event window
	 */
	enum EType
	{
		EVENT_NONE,				/**< No event */
		EVENT_KEY_PRESSED,		/**< Key pressed */
		EVENT_KEY_RELEASED,		/**< Key released */
		EVENT_MOUSE_PRESSED,	/**< Mouse pressed */
		EVENT_MOUSE_RELEASED,	/**< Mouse released */
		EVENT_MOUSE_MOVE,		/**< Mouse move */
		EVENT_MOUSE_WHEEL,		/**< Mouse wheel moved */
		EVENT_TEXT_INPUT		/**< Input text in window */
	};

	/**
	 * @brief Key pressed/released event
	 */
	struct KeyEvent
	{
		EButtonCode	code;			/**< Code of key */
		bool		bAlt;			/**< Is event with alt */
		bool		bControl;		/**< Is event with ctrl */
		bool		bShift;			/**< Is event with shift */
		bool		bSuper;			/**< Is event with super (key Windows on keyboard) */
		bool		bCapsLock;		/**< Is event with caps lock */
		bool		bNumLock;		/**< Is event with num lock */
	};

	/**
	 * @brief Mouse button pressed/released event
	 */
	struct MouseButtonEvent
	{
		EButtonCode	code;		/**< Code of mouse button */
		int32		x;			/**< Mouse position by X */
		int32		y;			/**< Mouse position by Y */
	};

	/**
	 * @brief Mouse moving event
	 */
	struct MouseMoveEvent
	{
		int32		x;					/**< Current position by X */
		int32		y;					/**< Current position by Y */
		int32		xDirection;			/**< Offset by X */
		int32		yDirection;			/**< Offset by Y */
	};

	/**
	 * @brief Mouse wheel moving event
	 */
	struct MouseWheelEvent
	{
		int32		x;					/**< Moving wheel by X */
		int32		y;					/**< Moving wheel by Y */
	};

	/**
	 * @brief Text input event
	 */
	struct TextInputEvent
	{
		achar*		pText;				/**< Entered text */
	};

	/**
	 * @brief Events union
	 */
	union UEvents
	{
		KeyEvent			key;				/**< Key pressed/released event */
		MouseButtonEvent	mouseButton;		/**< Mouse button pressed/released event */
		MouseMoveEvent		mouseMove;			/**< Mouse moving event */
		MouseWheelEvent		mouseWheel;			/**< Mouse wheel moving event */
		TextInputEvent		textInputEvent;		/**< Input text event */
	};

	/**
	 * @brief Constructor
	 */
	InputEvent()
		: type( EVENT_NONE )
	{}

	EType		type;	/**< Type event */
	UEvents		events;	/**< Events */
};

#endif // !INPUTEVENT_H