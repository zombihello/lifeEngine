/**
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

#include "pch_game.h"
#include "game/game.h"

/**
 * @ingroup game
 * @brief Eleot game
 */
class CEleotGame : public CGame
{
public:
	/**
	 * @brief Get game description
	 * @return Return string describing current module. For example: Eleot: Episodic, Nuclear Frost, etc
	 */
	virtual const achar* GetGameDescription() const override;
};

EXPOSE_SINGLE_INTERFACE( CEleotGame, IGame, GAME_INTERFACE_VERSION );
EXPOSE_SINGLE_INTERFACE( CGameAppSystems, IGameAppSystems, GAME_APPSYSTEMS_INTERFACE_VERSION );


/*
==================
CEleotGame::GetGameDescription
==================
*/
const achar* CEleotGame::GetGameDescription() const
{
	return "Eleot";
}