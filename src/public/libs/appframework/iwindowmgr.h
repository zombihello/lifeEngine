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

#ifndef IWINDOWMGR_H
#define IWINDOWMGR_H

#include "stdlib/delegate.h"
#include "appframework/iappsystem.h"
#include "appframework/windowevent.h"
#include "inputsystem/inputevent.h"

/**
 * @ingroup appframework
 * @brief Window manager interface version
 */
#define WINDOWMGR_INTERFACE_VERSION "LWindowMgr001"

/**
 * @ingroup appframework
 * @brief Delegate interface for process window event
 */
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnProcessWindowEvent, const WindowEvent& /* windowEvent */ );

/**
 * @ingroup appframework
 * @brief Delegate interface for process input event
 */
DECLARE_MULTICAST_DELEGATE_INTERFACE( IOnProcessInputEvent, const InputEvent& /* inputEvent */ );

/**
 * @ingroup appframework
 * @brief Enumeration of types window styles
 */
enum EStyleWindow
{
	WINDOW_STYLE_NONE			= 0,												/**< No styles */
	WINDOW_STYLE_RESIZABLE		= 1 << 0,											/**< Resizable window */
	WINDOW_STYLE_DECORATED		= 1 << 1,											/**< Decorated window  */
	WINDOW_STYLE_FLOATING		= 1 << 2,											/**< Floating window  */
	WINDOW_STYLE_FULLSCREEN		= 1 << 3,											/**< Fullscreen window  */
	WINDOW_STYLE_HIDDEN			= 1 << 4,											/**< Hidden window */
	WINDOW_STYLE_MINIMIZED		= 1 << 5,											/**< Window is minimized. If set flag WINDOW_STYLE_MAXIMIZED or WINDOW_STYLE_FULLSCREEN will be ignore */
	WINDOW_STYLE_MAXIMIZED		= 1 << 6,											/**< Window is maximized. If set flag WINDOW_STYLE_MINIMIZED or WINDOW_STYLE_FULLSCREEN will be ignore */

	WINDOW_STYLE_DEFAULT		= WINDOW_STYLE_DECORATED | WINDOW_STYLE_RESIZABLE	/**< Default style - combined decorated and resizable style  */
};

/**
 * @ingroup appframework
 * @brief Window manager interface
 */
class IWindowMgr : public IAppSystem
{
public:
	/**
	 * @brief Create window
	 *
	 * @param pTitle	Title
	 * @param width		Width
	 * @param height	Height
	 * @param flags		Combinations flags of EStyleWindow for set style of window
	 * @return Return TRUE if window is created successfully, otherwise will return FALSE
	 */
	virtual bool Create( const achar* pTitle, uint32 width, uint32 height, uint32 flags = WINDOW_STYLE_DEFAULT ) = 0;

	/**
	 * @brief Close window
	 */
	virtual void Close() = 0;

	/**
	 * @brief Show window
	 * @param bShowWindow	Is need show or hide window
	 */
	virtual void ShowWindow( bool bShowWindow = true ) = 0;

	/**
	 * @brief Maximize window
	 */
	virtual void Maximize() = 0;

	/**
	 * @brief Minimize window
	 */
	virtual void Minimize() = 0;

	/**
	 * @brief Process window events
	 */
	virtual void ProcessEvents() = 0;
	
	/**
	 * @brief Set title of window
	 * @param pTitle	New title
	 */
	virtual void SetTitle( const achar* pTitle ) = 0;

	/**
	 * @brief Set size of window
	 *
	 * @param width		Width
	 * @param height	Height
	 */
	virtual void SetSize( uint32 width, uint32 height ) = 0;

	/**
	 * @brief Set fullscreen mode
	 * @param bFullscreen	Enable or disable fullscreen mode
	 */
	virtual void SetFullscreen( bool bFullscreen ) = 0;

	/**
	 * @brief Get delegate of process window event
	 * @return Return delegate of process window event
	 */
	virtual IOnProcessWindowEvent* OnProcessWindowEvent() const = 0;

	/**
	 * @brief Get delegate of process input event
	 * @return Return delegate of process input event
	 */
	virtual IOnProcessInputEvent* OnProcessInputEvent() const = 0;

	/**
	 * @brief Is window open
	 * @return Return TRUE if window is opened, otherwise will return FALSE
	 */
	virtual bool IsOpen() const = 0;

	/**
	 * @brief Is enabled fullscreen mode
	 * @return Return TRUE if fullscreen mode is enabled, otherwise will return FALSE
	 */
	virtual bool IsFullscreen() const = 0;

	/**
	 * @brief Get size window
	 *
	 * @param width		Output window width
	 * @oaram height	Output window height
	 */
	virtual void GetSize( uint32& width, uint32& height ) const = 0;

	/**
	 * @brief Get window handle
	 * @return Return window handle
	 */
	virtual windowHandle_t GetHandle() const = 0;

	/**
	 * @brief Get ID window in the engine
	 * @return Return ID window in the engine. If window not created will return -1
	 */
	virtual uint32 GetID() const = 0;
};

/**
 * @ingroup appframework
 * @brief Create a window manager
 * @note Need implement on each platform
 * 
 * @return Return created a window manager
 */
IWindowMgr* CreateWindowMgr();

#endif // !IWINDOWMGR_H