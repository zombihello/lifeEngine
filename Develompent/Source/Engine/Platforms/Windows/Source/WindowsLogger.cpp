#include <stdio.h>
#include <ctime>

#include "LEBuild.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Containers/String.h"
#include "System/BaseFileSystem.h"
#include "WindowsLogger.h"

#if WITH_EDITOR
#include "System/EditorEngine.h"
#include "Misc/WorldEdGlobals.h"
#endif // WITH_EDITOR

const tchar* GLogTypeNames[] =
{
	TEXT( "Log" ),
	TEXT( "Warning" ),
	TEXT( "Error" )
};

const tchar* GLogCategoryNames[] =
{
	TEXT( "None" ),
	TEXT( "General" ),
	TEXT( "Init" ),
	TEXT( "Script" ),
	TEXT( "Dev" ),
	TEXT( "Shader" ),
	TEXT( "Input" ),
	TEXT( "Package" ),

#if WITH_EDITOR
	TEXT( "Editor" ),
	TEXT( "Commandlet" )
#endif // WITH_EDITOR
};

/**
 * Constructor
 */
FWindowsLogger::FWindowsLogger() :
	consoleHandle( nullptr ),
	archiveLogs( nullptr )
{}

/**
 * Destructor
 */
FWindowsLogger::~FWindowsLogger()
{}

/**
 * Shows or hides the console window
 */
void FWindowsLogger::Show( bool InShowWindow )
{
#if !NO_LOGGING
	if ( InShowWindow )
	{
		if ( consoleHandle )		return;

		AllocConsole();
		consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );

		freopen( "conin$", "r", stdin );
		freopen( "conout$", "w", stdout );
		freopen( "conout$", "w", stderr );
	}
	else
	{
		consoleHandle = nullptr;
		FreeConsole();
	}
#endif // !NO_LOGGING
}

/**
 * Initialize logger
 */
void FWindowsLogger::Init()
{
#if !NO_LOGGING
	time_t		timeNow = time( nullptr );
	tm*			tmTimeNow = localtime( &timeNow );

	std::wstring		logFile = FString::Format( TEXT( "../../Logs/%s-%i.%02i.%02i-%02i.%02i.%02i.log" ), !GIsEditor ? GGameName.c_str() : TEXT( "WorldEd" ), 1900 + tmTimeNow->tm_year, 1 + tmTimeNow->tm_mon, tmTimeNow->tm_mday, tmTimeNow->tm_hour, tmTimeNow->tm_min, tmTimeNow->tm_sec );
	archiveLogs = GFileSystem->CreateFileWriter( logFile.c_str(), AW_None );
	if ( archiveLogs )
	{
		archiveLogs->SetType( AT_TextFile );
		Logf( LT_Log, LC_Init, TEXT( "Opened log file '%s'" ), logFile.c_str() );
	}
#endif // !NO_LOGGING
}

/**
 * Closes output device and cleans up
 */
void FWindowsLogger::TearDown()
{
	Show( false );

	if ( archiveLogs )
	{
		delete archiveLogs;
		archiveLogs = nullptr;
	}
}

/**
 * Serialize message
 */
void FWindowsLogger::Serialize( const tchar* InMessage, ELogType InLogType, ELogCategory InLogCategory )
{
	// If console is opened - get current text color
	// and change to color by event type
	uint16				currentTextAttribute = 0;

	if ( consoleHandle )
	{
		CONSOLE_SCREEN_BUFFER_INFO		consoleScreenBufferInfo;
		if ( GetConsoleScreenBufferInfo( consoleHandle, &consoleScreenBufferInfo ) )
		{
			currentTextAttribute = consoleScreenBufferInfo.wAttributes;
		}

		// Cahnge color by event type
		switch ( InLogType )
		{
		case LT_Error:
			SetConsoleTextAttribute( consoleHandle, 0xC );		// Red color
			break;

		case LT_Warning:
			SetConsoleTextAttribute( consoleHandle, 0xE );		// Yealow color
			break;
		}
	}
	
	std::wstring			message = FString::Format( TEXT( "[%07.2f][%s][%s] %s" ), appSeconds() - GStartTime, GLogTypeNames[ ( uint32 ) InLogType ], GLogCategoryNames[ ( uint32 ) InLogCategory ], InMessage );
	std::wstring			finalMessage = message + TEXT( "\n" );
	wprintf( finalMessage.c_str() );

	// Print to log widget in WorldEd
#if WITH_EDITOR
	if ( GEditorEngine )
	{
		GEditorEngine->PrintLogToWidget( InLogType, message.c_str() );
	}
#endif // WITH_EDITOR

	// Serialize log to file
	if ( archiveLogs )
	{
		*archiveLogs << finalMessage;
		archiveLogs->Flush();
	}

	// Print message to debug output
#if !SHIPPING_BUILD
	if ( appIsDebuggerPresent() )
	{
		OutputDebugStringW( finalMessage.c_str() );
	}
#endif // !SHIPPING_BUILD

	// Change text attribute to default
	if ( consoleHandle )
	{
		SetConsoleTextAttribute( consoleHandle, currentTextAttribute );
	}
}