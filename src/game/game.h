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

#ifndef GAME_H
#define GAME_H

#include <vector>

#include "appframework/iappsystemgroup.h"
#include "game/igame.h"

/**
 * @ingroup game
 * @brief Base class of the game
 */
class CGame : public IGame
{
public:
	/**
	 * @brief Initialize the game DLL
	 *
	 * @param pFactory	Pointer to interface factory
	 * @return Return TRUE if the game is initialized, otherwise returns FALSE
	 */
	virtual bool Init( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Shutdown the game DLL
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Process one game frame
	 */
	virtual void FrameUpdate() override;
};

/**
 * @ingroup game
 * @brief Base class of the game IAppSystems
 */
class CGameAppSystems : public IGameAppSystems
{
public:
	/**
	 * @brief Get number of IAppSystems
	 * @return Return number of IAppSystems
	 */
	virtual uint32 GetNum() const override;

	/**
	 * @brief Get module name
	 *
	 * @param index		IAppSystem index
	 * @return Return module name
	 */
	virtual const achar* GetModuleName( uint32 index ) const override;

	/**
	 * @brief Get interface name
	 *
	 * @param index		IAppSystem index
	 * @return Return interface name
	 */
	virtual const achar* GetInterfaceName( uint32 index ) const override;

protected:
	/**
	 * @brief Add app system
	 * 
	 * @param pModuleName		Module name
	 * @param pInterfaceName	Interface name
	 */
	FORCEINLINE void AddAppSystem( const achar* pModuleName, const achar* pInterfaceName )
	{
		AppSystemInfo&					appSystemInfo = appSystems.emplace_back();
		appSystemInfo.pModuleName		= pModuleName;
		appSystemInfo.pInterfaceName	= pInterfaceName;
	}

private:
	std::vector<AppSystemInfo>		appSystems;		/**< Array of app systems */
};

#endif // GAME_H