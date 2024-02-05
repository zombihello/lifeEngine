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

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include "Core/Misc/Types.h"
#include "Core/System/WindowEvent.h"
#include "Core/CoreDefines.h"

/**
 * @ingroup Core
 * @brief Enumeration of types window styles
 */
enum EStyleWindow
{
	SW_None				= 0,											/**< No styles */
	SW_Resizable		= 1 << 0,										/**< Resizable window */
	SW_Decorated		= 1 << 1,										/**< Decorated window  */
	SW_Floating			= 1 << 2,										/**< Floating window  */
	SW_Fullscreen		= 1 << 3,										/**< Fullscreen window  */
	SW_Hidden			= 1 << 4,										/**< Hidden window */
	SW_Minimized		= 1 << 5,										/**< Window is minimized. If set flag SW_Maximized or SW_Fullscreen will be ignore */
	SW_Maximized		= 1 << 6,										/**< Window is maximized. If set flag SW_Minimized or SW_Fullscreen will be ignore */

	SW_Default			= SW_Decorated | SW_Resizable | SW_Hidden		/**< Default style - combined decorated and resizable style  */
};

/**
 * @ingroup Core
 * @brief The base class for work with window
 */
class CBaseWindow
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CBaseWindow() {}

	/**
	 * @brief Create window
	 * 
	 * @param[in] InTitle Title
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 * @param[in] InFlags Combinations flags of EStyleWindow for set style of window
	 */
	virtual void Create( const tchar* InTitle, uint32 InWidth, uint32 InHeight, uint32 InFlags = SW_Default ) {}

	/**
	 * @brief Close window
	 */
	virtual void Close() {}

	/**
	 * @brief Show cursor
	 */
	virtual void ShowCursor() {}

	/**
	 * @brief Hide cursor
	 */
	virtual void HideCursor() {}

	/**
	 * @brief Show window
	 */
	virtual void Show() {}

	/**
	 * @brief Hide window
	 */
	virtual void Hide() {}

	/**
	 * @brief Maximize window
	 */
	virtual void Maximize() {}

	/**
	 * @brief Minimize window
	 */
	virtual void Minimize() {}

	/**
	 * @brief Handle window event
	 * 
	 * @param[out] OutWindowEvent Occurred window event
	 * @return True if queue of events not empty, else false
	 */
	virtual bool PollEvent( WindowEvent& OutWindowEvent ) { return false; }
	
	/**
	 * @brief Set title of window
	 *
	 * @param[in] InTitle New title
	 */
	virtual void SetTitle( const tchar* InTitle ) {}

	/**
	 * @brief Set size of window
	 *
	 * @param[in] InWidth Width
	 * @param[in] InHeight Height
	 */
	virtual void SetSize( uint32 InWidth, uint32 InHeight ) {}

	/**
	 * @brief Set fullscreen mode
	 *
	 * @param[in] InIsFullscreen Enable fullscreen mode or disable
	 */
	virtual void SetFullscreen( bool InIsFullscreen ) {}

	/**
	 * @brief Is window open
	 * @return True if window oppened, else false
	 */
	virtual bool IsOpen() const { return false; }

	/**
	 * @brief Is showing cursor
	 * @return True cursor is showing, else false
	 */
	virtual bool IsShowingCursor() const { return false; }

	/**
	 * @brief Is enabled fullscreen mode
	 * @return True fullscreen is enabled, else false
	 */
	virtual bool IsFullscreen() const { return false; }

	/**
	 * @brief Get size window
	 *
	 * @param[out] OutWidth Width
	 * @oaram[out] OutHeight Height
	 */
	virtual void GetSize( uint32& OutWidth, uint32& OutHeight ) const {}

	/**
	 * @brief Get OS handle
	 * @return Return OS handle
	 */
	virtual WindowHandle_t GetHandle() const { return nullptr; }

	/**
	 * @brief Get ID window
	 * @return Return ID window. If window not created return ( uint32 )-1
	 */
	virtual uint32 GetID() const { return ( uint32 )-1; }
};

#endif // !BASEWINDOW_H
