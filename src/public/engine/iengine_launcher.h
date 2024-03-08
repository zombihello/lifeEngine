/**
 * @file
 * @addtogroup engine engine
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

#ifndef IENGINE_LAUNCHER_H
#define IENGINE_LAUNCHER_H

#include "appframework/iappsystem.h"

// Forward declarations
class CAppSystemGroup;

/**
 * @ingroup engine
 * @brief Main engine interface version to launcher 
 */
#define ENGINE_LAUNCHER_INTERFACE_VERSION "LEngineLauncher001"

/**
 * @ingroup engine
 * @brief Startup info
 */
struct StartupInfo
{
	void*				pAppInstance;				/**< OS application instance */
	const achar*		pGame;						/**< Root game name ("eleot", for example) */
	CAppSystemGroup*	pParentAppSystemGroup;		/**< Parent application system group */
};

/**
 * @ingroup engine
 * @brief Main engine interface to launcher
 */
class IEngineLauncher : public IAppSystem
{
public:
	/**
	 * @brief Set startup info
	 * @note This function must be called before Init()
	 *
	 * @param info	Startup info
	 */
	virtual void SetStartupInfo( const StartupInfo& info ) = 0;

	/**
	 * @brief Run the engine
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Run() = 0;
};

#endif // !IENGINE_LAUNCHER_H