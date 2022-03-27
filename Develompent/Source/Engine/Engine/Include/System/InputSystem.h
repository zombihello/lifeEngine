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
};

/**
 * @ingroup Engine
 * Class for work with input system
 */
class FInputSystem
{
public:
	/**
	 * Constructor
	 */
	FInputSystem();

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
	 * Get mouse location on screen
	 * @return Return mouse location on screen
	 */
	FORCEINLINE FVector2D GetMouseLocation() const
	{
		return mouseLocation;
	}

	/**
	 * Get mouse offset on current frame
	 * @return Return mouse offset on current frame
	 */
	FORCEINLINE FVector2D GetMouseOffset() const
	{
		return mouseOffset;
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
	FVector2D							mouseLocation;					/**< Location mouse on screen */
	FVector2D							mouseOffset;					/**< Offset mouse on current frame */
	float								mouseSensitivity;				/**< Mouse sensitivity */
};

#endif // !INPUTSYSTEM_H