#include <stdio.h>

#include "WindowsOutputDevices.h"

const tchar* GEventNames[] =
{
	TEXT( "Log" ),
	TEXT( "Warning" ),
	TEXT( "Critical" ),
	TEXT( "Init" )
};

WindowsOutputDevice::WindowsOutputDevice() :
	handle( nullptr )
{}

WindowsOutputDevice::~WindowsOutputDevice()
{}

void WindowsOutputDevice::Show( bool InShowWindow )
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

void WindowsOutputDevice::Serialize( const tchar* InMessage, EEventType InEvent )
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
		switch ( InEvent )
		{
		case OutputDevice::EEventType::Critical:
			SetConsoleTextAttribute( handle, 0xC );		// Red color
			break;

		case OutputDevice::EEventType::Warning:
			SetConsoleTextAttribute( handle, 0xE );		// Yealow color
			break;
		}
	}
	
	wprintf( TEXT( "[%s] %s\n" ), GEventNames[ ( uint32 ) InEvent ], InMessage );
	// TODO BG yehor.pohuliaka: Need added output to debug window OutputDebugStringW( ... );
	
	// Change text attribute to default
	if ( handle )
	{
		SetConsoleTextAttribute( handle, currentTextAttribute );
	}
}