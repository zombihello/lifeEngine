/**
 * @file
 * @addtogroup Core Core
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

#include "Core/Misc/Types.h"
#include "Core/Misc/SharedPointer.h"
#include "Core/System/ButtonCode.h"
#include "Core/CoreDefines.h"

/**
 * @ingroup Core
 * @brief Struct for storage event of window
 */
struct WindowEvent
{
	/**
	 * @brief Enumeration of types event window
	 */
	enum EType
	{
		T_None,						/**< No event */
		T_WindowClose,				/**< Close window */
		T_WindowResize,				/**< Resize window */
		T_WindowFocusGained,		/**< Window is gained focus */
		T_WindowFocusLost,			/**< Window is lost focus */
		T_WindowMove,				/**< Window moved */
		T_KeyPressed,				/**< Key pressed */
		T_KeyReleased,				/**< Key released */
		T_MousePressed,				/**< Mouse pressed */
		T_MouseReleased,			/**< Mouse released */
		T_MouseMove,				/**< Mouse move */
		T_MouseWheel,				/**< Mouse wheel moved */
		T_TextInput,				/**< Input text in window */

#if WITH_IMGUI
		T_ImGUILayerHovered,		/**< ImGUI layer is hovered */
		T_ImGUILayerUnHovered,		/**< ImGUI layer is unhovered */
#endif // WITH_IMGUI
	};

	/**
	 * @brief Event of key pressed/released
	 */
	struct KeyEvent
	{
		EButtonCode			code;			/**< Code of key */
		bool				isAlt;			/**< Is event with alt */
		bool				isControl;		/**< Is event with ctrl */
		bool				isShift;		/**< Is event with shift */
		bool				isSuper;		/**< Is event with super (key Windows on keyboard) */
		bool				isCapsLock;		/**< Is event with caps lock */
		bool				isNumLock;		/**< Is event with num lock */
	};

	/**
	 * @brief Event of mouse button pressed/released
	 */
	struct MouseButtonEvent
	{
		EButtonCode			code;		/**< Code of mouse button */
		int32				x;			/**< Mouse position by X */
		int32				y;			/**< Mouse position by Y */
	};

	/**
	 * @brief Event of mouse moving
	 */
	struct MouseMoveEvent
	{
		int32		x;					/**< Current position by X */
		int32		y;					/**< Current position by Y */
		int32		xDirection;			/**< Offset by X */
		int32		yDirection;			/**< Offset by Y */
	};

	/**
	 * @brief Event of mouse wheel moving
	 */
	struct MouseWheelEvent
	{
		int32		x;					/**< Moving wheel by X */
		int32		y;					/**< Moving wheel by Y */
	};

	/**
	 * @brief Event of window resize
	 */
	struct WindowResizeEvent
	{
		uint32		windowId;			/**< Id of window */
		int32		width;				/**< New width window */
		int32		height;				/**< New height window */
	};

	/**
	 * Event of window close
	 */
	struct WindowCloseEvent
	{
		uint32		windowId;			/**< Id window */
	};

	/**
	 * Event of window focus gained
	 */
	struct WindowFocusGainedEvent
	{
		uint32		windowId;			/**< Id window */
	};

	/**
	 * Event of window focus lost
	 */
	struct WindowFocusLostEvent
	{
		uint32		windowId;			/**< Id window */
	};

	/**
	 * Event of window move
	 */
	struct WindowMoveEvent
	{
		uint32		windowId;			/**< Id window */
		int32		x;					/**< Current position by X */
		int32		y;					/**< Current position by Y */
	};

	/**
	 * @brief Event of text input
	 */
	struct TextInputEvent
	{
		achar*			text;			/**< Entered text */
	};

	/**
	 * @brief Union of events
	 */
	union UEvents
	{
		WindowCloseEvent			windowClose;		/**< Event of window close */
		WindowResizeEvent			windowResize;		/**< Event of window resize */
		WindowFocusGainedEvent		windowFocusGained;	/**< Event of window focus gained */
		WindowFocusLostEvent		windowFocusLost;	/**< Event of window focus lost */
		WindowMoveEvent			windowMove;			/**< Event of window move */
		KeyEvent					key;				/**< Event of key pressed/released */
		MouseButtonEvent			mouseButton;		/**< Event of mouse button pressed/released */
		MouseMoveEvent				mouseMove;			/**< Event of mouse moving */
		MouseWheelEvent			mouseWheel;			/**< Event of mouse wheel moving */
		TextInputEvent				textInputEvent;		/**< Event of input text */
	};

	/**
	 * @brief Constructor
	 */
	FORCEINLINE WindowEvent() 
		: type( T_None ) 
#if WITH_IMGUI
		, bImGUIEvent( false )
#endif // WITH_IMGUI
	{}

#if WITH_IMGUI
	bool								bImGUIEvent;	/**< Is ImGUI event (see CImGUILayer::PollImGUIEvents) */
	TWeakPtr<class CImGUILayer>			imGUILayer;		/**< ImGUI layer. Him is valid when process event by ImGUI layer */
#endif // WITH_IMGUI

	EType								type;			/**< Type of event */
	UEvents								events;			/**< Events */
};

#endif // !WINDOWEVENT_H
