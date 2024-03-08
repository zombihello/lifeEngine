/**
 * @file
 * @addtogroup gameinfo gameinfo
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

#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <string>
#include <vector>

#include "core/types.h"
#include "core/platform.h"
#include "stdlib/jsondoc.h"

/**
 * @ingroup gameinfo
 * @brief Game info search path
 */
struct GameInfoSearchPath
{
	std::string		id;		/**< Path ID */
	std::string		path;	/**< Path */
};

/**
 * @ingroup gameinfo
 * @brief Helper for work with gameinfo.txt files
 */
class CGameInfoDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CGameInfoDoc();

	/**
	 * @brief Load from file
	 * @warning For use StdLib must be connected by ConnectStdLib
	 * 
	 * @param pPath		Path to file
	 * @return Return TRUE when file success loaded, otherwise returns FALSE
	 */
	bool LoadFromFile( const achar* pPath );

	/**
	 * @brief Load from buffer
	 *
	 * @param pBuffer			Buffer. The buffer must be null terminated
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 * @return Return TRUE if buffer successfully loaded, otherwise returns FALSE
	 */
	bool LoadFromBuffer( const achar* pBuffer, const achar* pGameInfoPath );

	/**
	 * @brief Clear game info
	 */
	void Clear();

	/**
	 * @brief Get game name
	 * @return Return game name
	 */
	FORCEINLINE const std::string& GetGame() const
	{
		return game;
	}

	/**
	 * @brief Get search paths
	 * @return Return search paths
	 */
	FORCEINLINE const std::vector<GameInfoSearchPath>& GetSearchPaths() const
	{
		return searchPaths;
	}

	/**
	 * @brief Is loaded game info
	 * @return Return TRUE if game info is loaded, otherwise returns FALSE
	 */
	FORCEINLINE bool IsLoaded() const
	{
		return bLoaded;
	}

private:
	/**
	 * @brief Grab data from JSON file
	 * 
	 * @param jsonDoc			JSON document
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 * @return Return TRUE if successfully got data from JSON file, otherwise returns FALSE
	 */
	bool GrabData( const CJsonDoc& jsonDoc, const achar* pGameInfoPath );

	/**
	 * @brief Replace macros in string
	 * 
	 * @param string			String where need replace macros
	 * @param pGameInfoPath		Path to directory where gameinfo.txt is. This is need for replace |gameinfo_path|
	 */
	void ReplaceMacros( std::string& string, const achar* pGameInfoPath );

	bool								bLoaded;		/**< Is loaded gameinfo */
	std::string							game;			/**< Game name */
	std::vector<GameInfoSearchPath>		searchPaths;	/**< Search paths */
};

#endif // !GAMEINFO_H