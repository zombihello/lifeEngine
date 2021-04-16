#include <stdio.h>

#include "Containers/String.h"
#include "BaseFileSystem.h"
#include "WindowsLogger.h"

const tchar* GLogCategoryNames[] =
{
	TEXT( "None" ),
	TEXT( "General" )
};

/**
 * Constructor
 */
WindowsLogger::WindowsLogger() :
	handle( nullptr ),
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
		if ( handle )		return;

		AllocConsole();
		handle = GetStdHandle( STD_OUTPUT_HANDLE );

		freopen( "conin$", "r", stdin );
		freopen( "conout$", "w", stdout );
		freopen( "conout$", "w", stderr );
	}
	else
	{
		handle = nullptr;
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

	if ( handle )
	{
		CONSOLE_SCREEN_BUFFER_INFO		consoleScreenBufferInfo;
		if ( GetConsoleScreenBufferInfo( handle, &consoleScreenBufferInfo ) )
		{
			currentTextAttribute = consoleScreenBufferInfo.wAttributes;
		}

		// Cahnge color by event type
		switch ( InLogType )
		{
		case LT_Error:
			SetConsoleTextAttribute( handle, 0xC );		// Red color
			break;

		case LT_Warning:
			SetConsoleTextAttribute( handle, 0xE );		// Yealow color
			break;
		}
	}
	
	tchar*			finalMessage = String::Format( TEXT( "[%s] %s\n" ), GLogCategoryNames[ ( uint32 )InLogCategory ], InMessage );
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
	if ( handle )
	{
		SetConsoleTextAttribute( handle, currentTextAttribute );
	}

	free( finalMessage );
}