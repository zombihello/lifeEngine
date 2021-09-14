#include <stdio.h>

#include "LEBuild.h"
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
	TEXT( "Init" ),
	TEXT( "Script" ),
	TEXT( "Dev" ),
	TEXT( "Shader" )
};

/**
 * Constructor
 */
FWindowsLogger::FWindowsLogger() :
	startLogging( std::chrono::steady_clock::now() ),
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
	archiveLogs = GFileSystem->CreateFileWriter( TEXT( "../../Launch.log" ), AW_None );
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
	
	tchar*			finalMessage = FString::Format( TEXT( "[%is][%s][%s] %s\n" ), std::chrono::duration_cast< std::chrono::seconds >( std::chrono::steady_clock::now() - startLogging ).count(), GLogTypeNames[ ( uint32 )InLogType ], GLogCategoryNames[ ( uint32 )InLogCategory ], InMessage );
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