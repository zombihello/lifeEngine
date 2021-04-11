#include "WindowsOutputDevices.h"
#include <stdio.h>

const tchar* GEventNames[] =
{
	TEXT( "Log" ),
	TEXT( "Warning" ),
	TEXT( "Critical" ),
	TEXT( "Init" )
};

void FWindowsOutputDevice::Serialize( const tchar* InMessage, EEventType InEvent )
{
	// TODO: BG yehor.pohulyaka - Implement logging with timestamp, coloring messages and fix bug with convert from ANSI to TCHAR
	wprintf( TEXT( "[%s] %s\n" ), GEventNames[ ( uint32 )InEvent ], InMessage );
}