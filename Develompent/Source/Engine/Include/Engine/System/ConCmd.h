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

#ifndef CONCMD_H
#define CONCMD_H

#include <string>
#include <vector>

#include "Core/System/Delegate.h"

/**
 * @ingroup Engine
 * @brief Console command
 */
class CConCmd
{
public:
	/**
	 * @brief Declare delegate of execute command
	 */
	DECLARE_DELEGATE( COnExecCmd, const std::vector<std::wstring>& /*InArgs*/ );

	/**
	 * @brief Constructor
	 * 
	 * @param InName			Name command
	 * @param InHelpText		Help text
	 * @param InExecDelegate	Execute delegate
	 */
	CConCmd( const std::wstring& InName, const std::wstring& InHelpText, const COnExecCmd::DelegateType_t& InExecDelegate );

	/**
	 * @brief Destructor
	 */
	~CConCmd();

	/**
	 * @brief Execute command
	 * @param InArgs	Array of arguments
	 */
	FORCEINLINE void Exec( const std::vector<std::wstring>& InArgs )
	{
		onExecCmd.Execute( InArgs );
	}

	/**
	 * @brief Get command name
	 * @return Return command name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get help text about this command
	 * @return Return help text about this command
	 */
	FORCEINLINE const std::wstring& GetHelpText() const
	{
		return helpText;
	}

private:
	std::wstring	name;			/**< Name */
	std::wstring	helpText;		/**< Help text */
	COnExecCmd		onExecCmd;		/**< Execute command delegate */
};

/**
 * @brief Get console commands
 * @return Return array of console commands
 */
FORCEINLINE std::vector<CConCmd*>& GetGlobalConCmds()
{
	static std::vector<CConCmd*>	cmds;
	return cmds;
}

#endif // !CONCMD_H