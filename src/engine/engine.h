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

#ifndef ENGINE_H
#define ENGINE_H

#include "core/platform.h"
#include "gameinfo/gameinfo.h"

/**
 * @ingroup engine
 * @brief The game engine
 */
class CEngine
{
public:
	/**
	 * @brief Constructor
	 */
	CEngine();

	/**
	 * @brief Initialize the game engine
	 * @return Return TRUE if the game engine successfully initialized, otherwise returns FALSE
	 */
	bool Init();

	/**
	 * @brief Shutdown the game engine
	 */
	void Shutdown();

	/**
	 * @brief Process one game frame
	 */
	void FrameUpdate();

	/**
	 * @brief Is initialized the game engine
	 * @return Return TRUE if the game engine initialized, otherwise returns FALSE
	 */
	FORCEINLINE bool IsInitialized() const
	{
		return bInitialized;
	}

	/**
	 * @brief Read file configuration
	 * Reads key bindings and archived cvars from pPath.
	 * If this file isn't exist try read from <filename>_default
	 * 
	 * @param pPath						Path to file
	 * @param bWriteConfigIfNoExist		Is need write a file if the one isn't exist
	 */
	void ReadConfiguration( const achar* pPath, bool bWriteConfigIfNoExist = true );

	/**
	 * @brief Write file configuration
	 * Writes key bindings and archived cvars into file
	 * 
	 * @param pPath		Path to file
	 */
	void WriteConfiguration( const achar* pPath );

	/**
	 * @brief Load gameinfo.txt
	 * 
	 * @param pPath		Path to gameinfo.txt
	 * @return Return TRUE if gameinfo loaded, otherwise returns FALSE
	 */
	FORCEINLINE bool LoadGameInfo( const achar* pPath )
	{
		return gameinfo.LoadFromFile( pPath );
	}

	/**
	 * @brief Unload gameinfo.txt
	 */
	FORCEINLINE void UnloadGameInfo()
	{
		gameinfo.Clear();
	}

	/**
	 * @brief Get gameinfo
	 * @return Return gameinfo
	 */
	FORCEINLINE const CGameInfoDoc& GetGameInfo() const
	{
		return gameinfo;
	}

private:
	/**
	 * @brief Override configuration with command line params
	 */
	void OverrideConfigurationFromCommandLine();

	bool			bInitialized;	/**< Is initialized the game engine */
	CGameInfoDoc	gameinfo;		/**< Game info */
};

/**
 * @ingroup engine
 * @brief The game engine
 */
extern CEngine		g_Engine;

#endif // !ENGINE_H