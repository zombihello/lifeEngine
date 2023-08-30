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
#include "Misc/SharedPointer.h"
#include "System/ButtonCode.h"
#include "CoreDefines.h"

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
