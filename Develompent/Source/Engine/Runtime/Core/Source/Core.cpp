#include "Core.h"
#include "OutputDevices.h"

void VARARGS appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    tchar       tempString[ 4096 ];
    GET_VARARGS( tempString, 4096, 4095, InFormat );
    GLog->Logf( FOutputDevice::EEventType::Critical, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), InExpr, InFile, InLine, tempString );
}

void VARARGS appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    tchar       tempString[ 4096 ];
    GET_VARARGS( tempString, 4096, 4095, InFormat );
    GLog->Logf( FOutputDevice::EEventType::Critical, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), InExpr, InFile, InLine, tempString );
}