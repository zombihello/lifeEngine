/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_H
#define WORLDED_H

#include <string>
#include <qdir.h>
#include <qlabel.h>
#include <qstring.h>
#include <qmessagebox.h>

#include "Misc/Types.h"

/**
 * @ingroup WorldEd
 * Main function for start WorldEd
 * 
 * @param[in] InCmdLine Arguments of start
 * @return Returns the error level, 0 if successful and > 0 if there were errors
 */
int32 appWorldEdEntry( const tchar* InCmdLine );

/**
 * @ingroup WorldEd
 * Get WorldEd name
 * 
 * @return Return WorldEd name
 */
std::wstring appGetWorldEdName();

/**
 * @ingroup WorldEd
 * Get engine path from Qt absolute path
 * 
 * @param InPath Absolute path
 * @return Return engine path
 */
FORCEINLINE std::wstring appQtAbsolutePathToEngine( const QString& InPath )
{
	static QDir			baseDir( "./" );
	return baseDir.relativeFilePath( InPath ).toStdWString();
}

/**
 * @ingroup WorldEd
 * Show message box with list
 * 
 * @param InParent		Parent window
 * @param InTitle		Title message box
 * @param InText		Message
 * @param InListName	List name
 * @param InList		List
 * @param InIsError		Is error message
 * @param InMaxSizeList	Max show items in list
 */
QMessageBox::StandardButton ShowMessageBoxWithList( class QWidget* InParent, const QString& InTitle, const QString& InText, const QString& InListName, const std::vector< QString >& InList, bool InIsError = false, uint32 InMaxSizeList = 3 );

#endif // !WORLDED_H