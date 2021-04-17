#include <stdio.h>

#include "Containers/String.h"
#include "System/BaseFileSystem.h"
#include "WindowsLogger.h"

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
	TEXT( "Init" )
};

/**
 * Constructor
 */
WindowsLogger::WindowsLogger() :
	startLogging( std::chrono::steady_clock::now() ),
	consoleHandle( nullptr ),
	archiveLogs( nullptr )
{}

/**
 * Destructor
 */
WindowsLogger::~WindowsLogger()
{}

/**
 * Shows or hides the console window
 */
void WindowsLogger::Show( bool InShowWindow )
{
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
}

/**
 * Initialize logger
 */
void WindowsLogger::Init()
{
	archiveLogs = GFileSystem->CreateFileWriter( TEXT( "Launch.log" ), AW_None );
}

/**
 * Closes output device and cleans up
 */
void WindowsLogger::TearDown()
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
void WindowsLogger::Serialize( const tchar* InMessage, ELogType InLogType, ELogCategory InLogCategory )
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
	
	tchar*			finalMessage = String::Format( TEXT( "[%is][%s][%s] %s\n" ), std::chrono::duration_cast< std::chrono::seconds >( std::chrono::steady_clock::now() - startLogging ).count(), GLogTypeNames[ ( uint32 )InLogType ], GLogCategoryNames[ ( uint32 )InLogCategory ], InMessage );
	wprintf( finalMessage );

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
		OutputDebugStringW( finalMessage );
	}
#endif // !SHIPPING_BUILD

	// Change text attribute to default
	if ( consoleHandle )
	{
		SetConsoleTextAttribute( consoleHandle, currentTextAttribute );
	}

	free( finalMessage );
}