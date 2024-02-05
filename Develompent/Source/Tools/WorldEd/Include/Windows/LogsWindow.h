/**
 * @file
 * @addtogroup WorldEd World editor
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

#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <string>
#include <vector>

#include "Core/Containers/StringConv.h"
#include "Core/Logger/LoggerMacros.h"
#include "UI/ImGUI/ImGUIEngine.h"

/**
 * @ingroup WorldEd
 * @brief Window logs
 */
class CLogsWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CLogsWindow( const std::wstring& InName );

	/**
	 * @brief Print log
	 *
	 * @param InLogType Log type
	 * @param InMessage Message
	 */
	FORCEINLINE void PrintLog( ELogType InLogType, const tchar* InMessage )
	{
		LogInfo		logInfo;
		logInfo.type = InLogType;
		logInfo.message = TCHAR_TO_ANSI( InMessage );
		history.push_back( logInfo );
	}

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Struct log info
	 */
	struct LogInfo
	{
		ELogType			type;			/**< Log type */
		std::string			message;		/**< Message */
	};

	/**
	 * @brief Execute console command
	 * @param InCommand		Console command
	 */
	void ExecCommand( const std::string& InCommand );

	std::vector<LogInfo>	history;		/**< Array of log history */
	std::string				commandBuffer;	/**< Command buffer */
};

#endif // !LOGSWINDOW_H