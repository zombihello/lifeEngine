#include <qapplication.h>
#include <qstring.h>
#include <qfile.h>
#include <qtextstream.h>
#include <string>
#include <vector>
#include <sstream>

#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/SplashScreen.h"
#include "System/EditorEngine.h"
#include "Misc/EngineGlobals.h"
#include "Misc/LaunchGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/Misc.h"
#include "RHI/BaseRHI.h"
#include "EngineLoop.h"
#include "WorldEd.h"

/**
 * Globals
 */
SEditorDelegates::COnAssetsCanDelete	SEditorDelegates::onAssetsCanDelete;
SEditorDelegates::COnAssetsDeleted		SEditorDelegates::onAssetsDeleted;
SEditorDelegates::COnAssetsReloaded		SEditorDelegates::onAssetsReloaded;
SEditorDelegates::COnWorldLoaded		SEditorDelegates::onWorldLoaded;

void QtMessageOutput( QtMsgType InType, const QMessageLogContext& InContext, const QString& InMessage )
{
	std::wstring		message = InMessage.toStdWString();
	switch ( InType )
	{
	case QtDebugMsg:
	case QtInfoMsg:
		LE_LOG( LT_Log, LC_Editor, message.c_str() );
		break;

	case QtWarningMsg:
		LE_LOG( LT_Warning, LC_Editor, message.c_str() );
		break;

	case QtCriticalMsg:
		LE_LOG( LT_Error, LC_Editor, message.c_str() );
		break;

	case QtFatalMsg:
		GEditorEngine->PrintLogToWidget( LT_Error, message.c_str() );
		appErrorf( message.c_str() );
		break;
	}
}

int32 appWorldEdEntry( const tchar* InCmdLine )
{
	// Parse arguments of start for create QApplication
	std::stringstream		strStream;
	std::vector< achar* >	arguments;
	strStream << TCHAR_TO_ANSI( InCmdLine );

	while ( !strStream.eof() )
	{
		std::string		tempString;
		strStream >> tempString;

		achar*		newArgument = new achar[ tempString.size()+1 ];
		strcpy( newArgument, tempString.data() );
		arguments.push_back( newArgument );
	}

	LE_LOG( LT_Log, LC_Init, TEXT( "Qt version: %s" ), ANSI_TO_TCHAR( qVersion() ) );

	// Create object of QApplication
	qInstallMessageHandler( QtMessageOutput );
	int32					argumentsCount = ( int32 )arguments.size();	
	QApplication			application( argumentsCount, arguments.data() );

	// Load styles and init engine
	appShowSplash( GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "Splash" ) ).GetString().c_str() );

	{
		QFile			file( TCHAR_TO_ANSI( ( appBaseDir() + TEXT( "Engine/Editor/Styles/Default/Style.css" ) ).c_str() ) );
		if ( file.exists() )
		{
			file.open( QFile::ReadOnly | QFile::Text );
			QTextStream			textStream( &file );
			application.setStyleSheet( textStream.readAll() );
		}
	}

	GEditorEngine = GEngine->Cast< CEditorEngine >();
	checkMsg( GEditorEngine, TEXT( "Class of engine for editor must be inherited from CEditorEngine" ) );

	int32		result = GEngineLoop->Init( InCmdLine );
	check( result == 0 );

	// Free allocated memory for arguments
	for ( uint32 index = 0, count = argumentsCount; index < argumentsCount; ++index )
	{
		delete arguments[ index ];
	}
	arguments.clear();

	// Start Qt execute
	appHideSplash();
	result = application.exec();
	GEditorEngine->Shutdown();
	GEditorEngine = nullptr;
	return result;
}

std::wstring appGetWorldEdName()
{
#if PLATFORM_WINDOWS
#if _WIN64
	const std::wstring				platformBitsString( TEXT( "64" ) );
#else
	const std::wstring				platformBitsString( TEXT( "32" ) );
#endif // _WIN64
#else
#error Insert court bitness of your platform
#endif // PLATFORM_WINDOWS

	return ÑString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );
}

QMessageBox::StandardButton ShowMessageBoxWithList( class QWidget* InParent, const QString& InTitle, const QString& InText, const QString& InListName, const std::vector<QString>& InList, bool InIsError /* = false */, uint32 InMaxSizeList /* = 3 */ )
{
	QString		resultList;

	// Forming a list
	for ( uint32 index = 0, count = InList.size(); index < count; ++index )
	{
		resultList += InList[ index ];
		if ( count > InMaxSizeList && index + 1 == InMaxSizeList )
		{
			resultList += "<br>...<br>";
			break;
		}
		else
		{
			resultList += "<br>";
		}
	}

	QString		finalText = QString::fromStdWString( ÑString::Format( TEXT( "%s<br><br><b>%s:</b><br>%s" ), InText.toStdWString().c_str(), InListName.toStdWString().c_str(), resultList.toStdWString().c_str() ) );
	if ( InIsError )
	{
		return QMessageBox::critical( InParent, InTitle, finalText, QMessageBox::Ok );
	}
	else
	{
		return QMessageBox::warning( InParent, InTitle, finalText, QMessageBox::Ok );
	}
}