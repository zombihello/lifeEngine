/**
 * @file
 * @addtogroup appframework appframework
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

#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H

#include "core/types.h"

/**
 * @ingroup appframework
 * @brief Struct for storage event of window
 */
struct WindowEvent
{
	/**
	 * @brief Enumeration of types event window
	 */
	enum EType
	{
		EVENT_NONE,					/**< No event */
		EVENT_WINDOW_CLOSE,			/**< Close window */
		EVENT_WINDOW_RESIZE,		/**< Resize window */
		EVENT_WINDOW_FOCUS_GAINED,	/**< Window is gained focus */
		EVENT_WINDOW_FOCUS_LOST,	/**< Window is lost focus */
		EVENT_WINDOW_MOVE			/**< Window moved */
	};

	/**
	 * @brief Window resize event
	 */
	struct WindowResizeEvent
	{
		uint32		windowId;	/**< Id window */
		int32		width;		/**< New width window */
		int32		height;		/**< New height window */
	};

	/**
	 * @brief Window close event
	 */
	struct WindowCloseEvent
	{
		uint32		windowId;	/**< Id window */
	};

	/**
	 * @brief Window focus gained event
	 */
	struct WindowFocusGainedEvent
	{
		uint32		windowId;	/**< Id window */
	};

	/**
	 * @brief Window focus lost event
	 */
	struct WindowFocusLostEvent
	{
		uint32		windowId;	/**< Id window */
	};

	/**
	 * @brief Window move event
	 */
	struct WindowMoveEvent
	{
		uint32		windowId;	/**< Id window */
		int32		x;			/**< Current position by X */
		int32		y;			/**< Current position by Y */
	};

	/**
	 * @brief Events union
	 */
	union UEvents
	{
		WindowCloseEvent		windowClose;		/**< Window close event */
		WindowResizeEvent		windowResize;		/**< Window resize event */
		WindowFocusGainedEvent	windowFocusGained;	/**< Window focus gained event */
		WindowFocusLostEvent	windowFocusLost;	/**< Window focus lost event */
		WindowMoveEvent			windowMove;			/**< Window move event */
	};

	/**
	 * @brief Constructor
	 */
	WindowEvent() 
		: type( EVENT_NONE )
	{}

	EType		type;	/**< Type event */
	UEvents		events;	/**< Events */
};

#endif // !WINDOWEVENT_H