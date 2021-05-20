/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H

#include "Misc/Types.h"
#include "System/ButtonCode.h"

/**
 * @ingroup Core
 * @brief Struct for storage event of window
 */
struct SWindowEvent
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
		T_KeyPressed,				/**< Key pressed */
		T_KeyReleased,				/**< Key released */
		T_MousePressed,				/**< Mouse pressed */
		T_MouseReleased,			/**< Mouse released */
		T_MouseMove,				/**< Mouse move */
		T_MouseWheel,				/**< Mouse wheel moved */
		T_TextInput					/**< Input text in window */
	};

	/**
	 * @brief Event of key pressed/released
	 */
	struct SKeyEvent
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
	struct SMouseButtonEvent
	{
		EButtonCode			code;		/**< Code of mouse button */
		int32				x;			/**< Mouse position by X */
		int32				y;			/**< Mouse position by Y */
	};

	/**
	 * @brief Event of mouse moving
	 */
	struct SMouseMoveEvent
	{
		int32		x;					/**< Current position by X */
		int32		y;					/**< Current position by Y */
		int32		xDirection;			/**< Offset by X */
		int32		yDirection;			/**< Offset by Y */
	};

	/**
	 * @brief Event of mouse wheel moving
	 */
	struct SMouseWheelEvent
	{
		int32		x;					/**< Moving wheel by X */
		int32		y;					/**< Moving wheel by Y */
	};

	/**
	 * @brief Event of window resize
	 */
	struct SWindowResizeEvent
	{
		int32		width;				/**< New width window */
		int32		height;				/**< New height window */
	};

	/**
	 * @brief Event of text input
	 */
	struct STextInputEvent
	{
		achar*			text;			/**< Entered text */
	};

	/**
	 * @brief Union of events
	 */
	union UEvents
	{
		SWindowResizeEvent			windowResize;		/**< Event of window resize */
		SKeyEvent					key;				/**< Event of key pressed/released */
		SMouseButtonEvent			mouseButton;		/**< Event of mouse button pressed/released */
		SMouseMoveEvent				mouseMove;			/**< Event of mouse moving */
		SMouseWheelEvent			mouseWheel;			/**< Event of mouse wheel moving */
		STextInputEvent				textInputEvent;		/**< Event of input text */
	};

	/**
	 * @brief Constructor
	 */
						SWindowEvent() : type( T_None ) {}

	EType			type;			/**< Type of event */
	UEvents			events;			/**< Events */
};

#endif // !WINDOWEVENT_H
