/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include "Core/Core.h"

/**
 * @ingroup Engine
 * SplashTextType defines the types of text on the splash screen
 */
enum ESplashTextType
{
	STT_StartupProgress = 0,		/**< Startup progress text */
	STT_VersionInfo1,				/**< Version information text line 1 */
	STT_CopyrightInfo,				/**< Copyright information text */
	STT_GameName,					/**< Game name */
	STT_NumTextTypes				/**< Number of text types (must be final enum value) */
};

/**
 * @ingroup Engine
 * Displays a splash window with the specified image. The function does not use wxWidgets. The splash
 * screen variables are stored in static global variables (SplashScreen*). If the image file doesn't exist,
 * the function does nothing.
 *
 * @param[in] InSplashName Name of splash screen file (and relative path if needed)
 */
extern void Sys_ShowSplash( const tchar* InSplashName );

/**
 * @ingroup Engine
 * Destroys the splash window that was previously shown by Sys_ShowSplash(). If the splash screen isn't active,
 * it will do nothing.
 */
extern void Sys_HideSplash();

/**
 * @ingroup Engine
 * Sets the text displayed on the splash screen (for startup/loading progress)
 *
 * @param[in] InType Type of text to change
 * @param[in] InText Text to display
 */
extern void Sys_SetSplashText( const ESplashTextType InType, const tchar* InText );

#endif // !SPLASHSCREEN_H