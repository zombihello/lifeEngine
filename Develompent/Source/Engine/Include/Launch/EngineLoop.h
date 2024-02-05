/**
 * @file
 * @addtogroup Launch Launch
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

#ifndef ENGINELOOP_H
#define ENGINELOOP_H

#include "Core/Misc/Types.h"
#include "Engine/System/BaseEngine.h"

/**
 * @ingroup Launch
 * @brief Typedef of pointer to function for create engine instance
 */
typedef CBaseEngine* ( *CreateEngineFn_t )();

/**
 * @ingroup Launch
 * @brief Class for initialize and tick engine
 */
class CEngineLoop
{
public:
	friend void Sys_ProcessWindowEvents();

	/**
	 * @brief Constructor
	 * @param InCreateEngineFn	Pointer to function for create engine instance
	 */
	CEngineLoop( CreateEngineFn_t InCreateEngineFn );

	/**
	 * @brief Run the engine
	 * 
	 * @param InCmdLine		Command line
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32 Run( const tchar* InCmdLine );

private:
	/**
	 * @brief Pre-Initialize the main loop
	 *
	 * @param InCmdLine		Command line
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32 PreInit( const tchar* InCmdLine );

	/**
	 * @brief Initialize the main loop
	 *
	 * @return Returns the error level, 0 if successful and > 0 if there were errors
	 */
	int32 Init();

	/**
	 * @brief Init configs
	 */
	void InitConfigs();

	/**
	 * @brief Advances main loop
	 */
	void Tick();

	/**
	 * @brief Performs shut down
	 */
	void Exit();

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent		Window event
	 */
	void ProcessEvent( struct WindowEvent& InWindowEvent );

	bool				isInitialize;		/**< Is initialized engine */
	bool				bIsFocus;			/**< Is focus on window */
	CreateEngineFn_t	CreateEngine;		/**< Pointer to function for create engine instance */
};

#endif // !ENGINELOOP_H
