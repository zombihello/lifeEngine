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

#ifndef IINPUTSYSTEM_H
#define IINPUTSYSTEM_H

#include "stdlib/math/math.h"
#include "appframework/iappsystem.h"
#include "appframework/iwindowmgr.h"
#include "inputsystem/buttoncode.h"

/**
 * @ingroup inputsystem
 * @brief Enumeration of type button events
 */
enum EButtonEvent
{
	BUTTON_EVENT_NONE,			/**< No event */
	BUTTON_EVENT_PRESSED,		/**< Pressed button */
	BUTTON_EVENT_RELEASED,		/**< Released button */
	BUTTON_EVENT_SCROLLED,		/**< Button is scrolled */
	BUTTON_EVENT_MOVED,			/**< Button is moved */
};

/**
 * @ingroup inputsystem
 * @brief Input system interface version
 */
#define INPUTSYSTEM_INTERFACE_VERSION "LInputSystem001"

/**
 * @ingroup inputsystem
 * @brief Input system interface
 */
class IInputSystem : public IAppSystem
{
public:
	/**
	 * @brief Attach the input system to a window
	 * @param pWindowMgr	Window manager
	 */
	virtual void AttachToWindow( IWindowMgr* pWindowMgr ) = 0;

	/**
	 * @brief Detach the input system from the window
	 */
	virtual void DetachFromWindow() = 0;

	/**
	 * @brief Clears the input state
	 */
	virtual void ClearInputState() = 0;

	/**
	 * @brief Set binding a key
	 * 
	 * @param button	Button code
	 * @param pCommand	Console command will be executed when the button is triggered
	 */
	virtual void SetBinding( EButtonCode button, const achar* pCommand ) = 0;

	/**
	 * @brief Unbind all keys
	 */
	virtual void UnbindAll() = 0;

	/**
	 * @brief Set relative mouse mode
	 * 
	 * While the mouse is in relative mode, the cursor is hidden, and the
	 * driver will try to report continuous motion in the current window.
	 * Only relative motion events will be delivered, the mouse position
	 * will not change.
	 * 
	 * @param bEnabled	 Whether or not to enable relative mode
	 */
	virtual void SetRelativeMouseMode( bool bEnabled ) = 0;

	/**
	 * @brief Is key down
	 * 
	 * @param key	Key button
	 * @return Return TRUE if key have event 'pressed', otherwise returns FALSE
	 */
	virtual bool IsKeyDown( EButtonCode key ) const = 0;

	/**
	 * @brief Is key up
	 * 
	 * @param key	Key button
	 * @return Return TRUE if key have event 'released', otherwise returns FALSE
	 */
	virtual bool IsKeyUp( EButtonCode key ) const = 0;

	/**
	 * @brief Is mouse key down
	 *
	 * @param key	Key button
	 * @return Return TRUE if mouse key have event 'pressed', otherwise returns FALSE
	 */
	virtual bool IsMouseKeyDown( EButtonCode key ) const = 0;

	/**
	 * @brief Is mouse key up
	 *
	 * @param key	Key button
	 * @return Return TRUE if key have event 'released', otherwise returns FALSE
	 */
	virtual bool IsMouseKeyUp( EButtonCode key ) const = 0;

	/**
	 * @brief Is scrolled mouse wheel
	 *
	 * @param wheel		Side scrolled mouse wheel
	 * @return Return TRUE if wheel have event 'scrolled', otherwise returns FALSE
	 */
	virtual bool IsMouseWheel( EButtonCode wheel ) const = 0;

	/**
	 * @brief Is mouse moved
	 *
	 * @param mouseAxis		Axis of moved mouse
	 * @return Return TRUE if mouse moved by axis, otherwise returns FALSE
	 */
	virtual bool IsMouseMoved( EButtonCode mouseAxis ) const = 0;

	/**
	 * @brief Is relative mouse mode enabled
	 * @return Return TRUE if relative mouse mode enabled, otherwise FALSE
	 */
	virtual bool IsRelativeMouseMode() const = 0;

	/**
	 * @brief Get mouse location on screen
	 * @return Return mouse location on screen
	 */
	virtual Vector2D GetMouseLocation() const = 0;

	/**
	 * @brief Get mouse offset on current frame
	 * @return Return mouse offset on current frame
	 */
	virtual Vector2D GetMouseOffset() const = 0;

	/**
	 * @brief Get mouse offset by axis
	 *
	 * @param mouseAxis	Axis of moved mouse
	 * @return Return mouse offset by axis
	 */
	virtual float GetMouseOffset( EButtonCode mouseAxis ) const = 0;

	/**
	 * @brief Get mouse sensitivity
	 * @return Return mouse sensitivity
	 */
	virtual float GetMouseSensitivity() const = 0;

	/**
	 * @brief Get current event in button
	 *
	 * @param buttonCode	Button code
	 * @return Return current event in button
	 */
	virtual EButtonEvent GetButtonEvent( EButtonCode buttonCode ) const = 0;

	/**
	 * @brief Get button code by name
	 * 
	 * @param pButtonName	Button name
	 * @return Return button code, if button with name not found will return BUTTON_CODE_NONE
	 */
	virtual EButtonCode GetButtonCodeByName( const achar* pButtonName ) const = 0;

	/**
	 * @brief Get button name
	 * 
	 * @param buttonCode	Button code
	 * @return Return button name, if button code not valid will return empty string
	 */
	virtual const achar* GetButtonName( EButtonCode buttonCode ) const = 0;
};

#endif // !IINPUTSYSTEM_H