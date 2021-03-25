#include <stdarg.h>
#include <string.h>
#include <memory>

#include "OutputDevices.h"

void FOutputDevice::Logf( FOutputDevice::EEventType InEvent, const tchar* InMessage, ... )
{
    uint32          bufferSize = 1024;
    tchar*          buffer = nullptr;
    int             result = -1;

    while ( result == -1 )
    {
        free( buffer );
        buffer = ( tchar* )malloc( bufferSize * sizeof( tchar ) );

        va_list			argList;
        va_start( argList, InMessage );
        result = vswprintf( buffer, bufferSize, InMessage, argList );
        va_end( argList );

        if ( result >= bufferSize )
        {
            result = 1;
        }

        bufferSize *= 2;
    }
}

