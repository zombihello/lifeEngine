#include "Core.h"
#include "Containers/StringConv.h"
#include "Misc/OutputDevices.h"

void VARARGS appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    tchar       tempString[ 4096 ];
    GET_VARARGS( tempString, 4096, 4095, InFormat );
    GLog->Logf( OutputDevice::EEventType::Critical, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, tempString );
}

void VARARGS appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    tchar       tempString[ 4096 ];
    GET_VARARGS( tempString, 4096, 4095, InFormat );
    GLog->Logf( OutputDevice::EEventType::Critical, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, tempString );
}