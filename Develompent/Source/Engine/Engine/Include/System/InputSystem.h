/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "System/ButtonCode.h"
#include "Math/Math.h"

/**
 * @ingroup Engine
 * Enumeration of type button events
 */
enum EButtonEvent
{
	BE_None,		/**< No event */
	BE_Pressed,		/**< Pressed button */
	BE_Released,	/**< Released button */
	BE_Scrolled,	/**< Button is scrolled */
	BE_Moved,		/**< Button is moved */
};

/**
 * @ingroup Engine
 * Class for work with input system
 */
class CInputSystem
{
public:
	/**
	 * Constructor
	 */
	CInputSystem();

	/**
	 * Init input system
	 */
	void Init();

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * Reset all input events
	 */
	void ResetEvents();

	/**
	 * Is key down
	 * 
	 * @param[in] InKey Key button
	 */
	FORCEINLINE bool IsKeyDown( EButtonCode InKey ) const
	{
		if ( InKey < BC_KeyFirst || InKey > BC_KeyLast )
		{
			return false;
		}

		return buttonEvents[ InKey ] == BE_Pressed;
	}

	/**
	 * Is key up
	 * 
	 * @param[in] InKey Key button
	 */
	FORCEINLINE bool IsKeyUp( EButtonCode InKey ) const
	{
		if ( InKey < BC_KeyFirst || InKey > BC_KeyLast )
		{
			return false;
		}

		return buttonEvents[ InKey ] == BE_Released;
	}

	/**
	 * Is mouse key down
	 * 
	 * @param[in] InKey Key button
	 */
	FORCEINLINE bool IsMouseKeyDown( EButtonCode InKey ) const
	{
		if ( InKey < BC_MouseFirst || InKey > BC_MouseLast )
		{
			return false;
		}

		return buttonEvents[ InKey ] == BE_Pressed;
	}

	/**
	 * Is mouse key up
	 * 
	 * @param[in] InKey Key button
	 */
	FORCEINLINE bool IsMouseKeyUp( EButtonCode InKey ) const
	{
		if ( InKey < BC_MouseFirst || InKey > BC_MouseLast )
		{
			return false;
		}

		return buttonEvents[ InKey ] == BE_Released;
	}

	/**
	 * Is scrolled mouse wheel
	 * 
	 * @param[in] InWheel Side scrolled mouse wheel
	 */
	FORCEINLINE bool IsMouseWheel( EButtonCode InWheel ) const
	{
		if ( InWheel != BC_MouseWheelUp || InWheel != BC_MouseWheelDown )
		{
			return false;
		}

		return buttonEvents[ InWheel ] == BE_Scrolled;
	}

	/**
	 * Is mouse moved
	 * 
	 * @param InMouseAxis	Axis of moved mouse
	 * @return Return TRUE if mouse moved by axis
	 */
	FORCEINLINE bool IsMouseMoved( EButtonCode InMouseAxis ) const
	{
		if ( InMouseAxis != BC_MouseX || InMouseAxis != BC_MouseY )
		{
			return false;
		}

		return buttonEvents[ InMouseAxis ] == BE_Moved;
	}

	/**
	 * Get mouse location on screen
	 * @return Return mouse location on screen
	 */
	FORCEINLINE Vector2D GetMouseLocation() const
	{
		return mouseLocation;
	}

	/**
	 * Get mouse offset on current frame
	 * @return Return mouse offset on current frame
	 */
	FORCEINLINE Vector2D GetMouseOffset() const
	{
		return mouseOffset;
	}

	/**
	 * Get mouse offset by axis
	 * 
	 * @param InMouseAxis	Axis of moved mouse
	 * @return Return mouse offset by axis
	 */
	FORCEINLINE float GetMouseOffset( EButtonCode InMouseAxis ) const
	{
		switch ( InMouseAxis )
		{
		case BC_MouseX:		return GetMouseOffset( A_X );
		case BC_MouseY:		return GetMouseOffset( A_Y );
		default:			return 0.f;
		}
	}

	/**
	 * Get mouse offset by axis
	 *
	 * @param InAxis	Axis
	 * @return Return mouse offset by axis
	 */
	FORCEINLINE float GetMouseOffset( EAxis InAxis ) const
	{
		switch ( InAxis )
		{
		case A_X:		return mouseOffset.x;
		case A_Y:		return mouseOffset.y;
		default:		return 0.f;
		}
	}

	/**
	 * Get mouse sensitivity
	 * @return Return mouse sensitivity
	 */
	FORCEINLINE float GetMouseSensitivity() const
	{
		return mouseSensitivity;
	}

	/**
	 * Get current event in button
	 * 
	 * @param InButtonCode Button code
	 * @return Return current event in button
	 */
	FORCEINLINE EButtonEvent GetButtonEvent( EButtonCode InButtonCode ) const
	{
		return buttonEvents[ InButtonCode ];
	}

private:
	EButtonEvent						buttonEvents[ BC_Count ];		/**< Events button */
	Vector2D							mouseLocation;					/**< Location mouse on screen */
	Vector2D							mouseOffset;					/**< Offset mouse on current frame */
	float								mouseSensitivity;				/**< Mouse sensitivity */
};

#endif // !INPUTSYSTEM_H