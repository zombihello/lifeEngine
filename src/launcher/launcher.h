/**
 * @file
 * @addtogroup launcher launcher
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

#ifndef LAUNCHER_H
#define LAUNCHER_H

#include "appframework/iappsystemgroup.h"
#include "appframework/iwindowmgr.h"
#include "engine/iengine_launcher.h"

/**
 * @ingroup launcher
 * @brief Initialize of launcher's log output
 */
void Launcher_InitLogOutput();

/**
 * @ingroup launcher
 * @brief LifeEngine application
 */
class CLifeEngineApp : public CDefaultAppSystemGroup<CAppSystemGroup>
{
public:
	/**
	 * @brief Constructor
	 * @param pHInstance	Application instance handle
	 */
	CLifeEngineApp( appInstanceHandle_t hInstance = nullptr );

	/**
	 * @brief An installed application creation function, you should tell the group
	 * the DLLs and the singleton interfaces you want to instantiate.
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool Create() override;

	/**
	 * @brief Allow the application to do some work after AppSystems are connected
	 *
	 * Allow the application to do some work after AppSystems are connected but
	 * they aren't all Initialized
	 *
	 * @return Return FALSE if there's any problems and the app will abort
	 */
	virtual bool PreInit() override;

	/**
	 * @brief Main loop implemented by the application
	 * @return Return exit code. If all ok returns zero
	 */
	virtual int32 Main() override;

	/**
	 * @brief Allow the application to do some work after all AppSystems are shut down
	 */
	virtual void PostShutdown() override;

private:
	appInstanceHandle_t		hInstance;			/**< Application instance handle */
	IWindowMgr*				pWindowMgr;			/**< Window manager */
	IEngineLauncher*		pEngineLauncher;	/**< Engine launcher */
};

#endif // !LAUNCHER_H