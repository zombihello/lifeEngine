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

#ifndef CONSOLESYSTEM_H
#define CONSOLESYSTEM_H

#include <string>
#include <vector>

#include "Engine/System/ConCmd.h"
#include "Engine/System/ConVar.h"

/**
 * @ingroup Engine
 * @brief Console system
 */
class CConsoleSystem
{
public:
	/**
	 * @brief Execute command
	 * 
	 * @param InCommand		Command
	 * @return Return TRUE if command is successfully executed, otherwise will returning FALSE 
	 */
	bool Exec( const std::wstring& InCommand );

	/**
	 * @brief Find console variable
	 * 
	 * @param InName	Name of the console variable
	 * @return Return pointer to console variable is exist, otherwise will return NULL
	 */
	CConVar* FindVar( const std::wstring& InName );

	/**
	 * @brief Find console command
	 *
	 * @param InName	Name of the console command
	 * @return Return pointer to console command is exist, otherwise will return NULL
	 */
	CConCmd* FindCmd( const std::wstring& InName );

	/**
	 * @brief Command 'Help'
	 * 
	 * @param InArguments		Command arguments
	 */
	static void CmdHelp( const std::vector<std::wstring>& InArguments );
};

#endif // !CONSOLESYSTEM_H