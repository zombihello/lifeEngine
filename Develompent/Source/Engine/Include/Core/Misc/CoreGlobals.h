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

#ifndef COREGLOBALS_H
#define COREGLOBALS_H

#include <string>

/**
 * @ingroup Core
 * @brief Logger
 */
extern class CBaseLogger*			g_Log;

/**
 * @ingroup Core
 * @brief File system
 */
extern class CBaseFileSystem*		g_FileSystem;

/**
 * @ingroup Core
 * @brief Window
 */
extern class CBaseWindow*			g_Window;

/**
 * @ingroup Core
 * @brief Config manager
 */
extern class CConfigManager			g_Config;

/**
 * @ingroup Core
 * @brief Indicates that MainLoop() should be exited at the end of the current iteration
 */
extern bool							g_IsRequestingExit;

#if WITH_EDITOR
/**
 * @ingroup Core
 * @brief Whether we should pause before exiting
 */
extern bool							g_ShouldPauseBeforeExit;
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * @brief Seconds per CPU cycle for this PC 
 */
extern double						g_SecondsPerCycle;

/**
* @ingroup Core
* @brief Time at which Sys_Seconds() was first initialized (very early on)
*/
extern double						g_StartTime;

/**
 * @ingroup Core
 * @brief Current delta time in seconds
 */
extern double						g_DeltaTime;

/**
 * @ingroup Core
 * @brief Current time in seconds
 */
extern double						g_CurrentTime;

/**
 * @ingroup Core
 * @brief Last time in seconds
 */
extern double						g_LastTime;

/**
 * @ingroup Core
 * @brief Package manager
 */
extern class CPackageManager*		g_PackageManager;

/**
 * @ingroup Core
 * @brief Table of contents
 */
extern class CTableOfContets		g_TableOfContents;

/**
 * @ingroup Core
 * @brief Platform type
 */
extern enum EPlatformType			g_Platform;

/**
 * @ingroup Core
 * @brief Command line
 */
extern class CCommandLine			g_CommandLine;

/**
 * @ingroup Core
 * @brief Asset factory
 */
extern class CAssetFactory			g_AssetFactory;

#endif // !COREGLOBALS_H
