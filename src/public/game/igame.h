/**
 * @file
 * @addtogroup game game
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

#ifndef IGAME_H
#define IGAME_H

#include "stdlib/interface.h"

/**
 * @ingroup game
 * @brief Game interface version
 */
#define GAME_INTERFACE_VERSION "LGame001"

/**
 * @ingroup game
 * @brief Game interface
 */
class IGame
{
public:
	/**
	 * @brief Initialize the game DLL
	 * 
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if the game is initialized, otherwise returns FALSE
	 */
	virtual bool Init( CreateInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Shutdown the game DLL
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Process one game frame
	 */
	virtual void FrameUpdate() = 0;

	/**
	 * @brief Get game description
	 * @return Return string describing current module. For example: Eleot: Episodic, Nuclear Frost, etc
	 */
	virtual const achar* GetGameDescription() const = 0;
};

/**
 * @ingroup game
 * @brief Game IAppSystems interface version
 */
#define GAME_APPSYSTEMS_INTERFACE_VERSION "LGameAppSystems001"

/**
 * @ingroup game
 * @brief Interface exposed from the game module back to the engine for specifying IAppSystems
 */
class IGameAppSystems
{
public:
	/**
	 * @brief Get number of IAppSystems
	 * @return Return number of IAppSystems
	 */
	virtual uint32 GetNum() const = 0;

	/**
	 * @brief Get module name
	 * 
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const = 0;

	/**
	 * @brief Get interface name
	 * 
	 * @param index		IAppSystem index
	 * @return Return interface name
	 */
	virtual const achar* GetInterfaceName( uint32 index ) const = 0;
};

#endif // !IGAME_H