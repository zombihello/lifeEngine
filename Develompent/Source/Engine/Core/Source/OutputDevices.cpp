#include <memory>

#include "OutputDevices.h"

void FOutputDevice::Logf( FOutputDevice::EEventType InEvent, const tchar* InMessage, ... )
{
    int32           bufferSize = 1024;
    tchar*          buffer = nullptr;
    int             result = -1;

    while ( result == -1 )
    {
        free( buffer );
        buffer = ( tchar* )malloc( bufferSize * sizeof( tchar ) );
        GET_VARARGS_RESULT( buffer, bufferSize, bufferSize-1, InMessage, result );
        bufferSize *= 2;
    }
    buffer[ result ] = 0;

    Serialize( buffer, InEvent );
    free( buffer );
}

