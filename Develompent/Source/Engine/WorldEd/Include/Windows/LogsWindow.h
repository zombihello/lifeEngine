/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LOGSWINDOW_H
#define LOGSWINDOW_H

#include <string>
#include <vector>

#include "Misc/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "ImGUI/ImGUIEngine.h"

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