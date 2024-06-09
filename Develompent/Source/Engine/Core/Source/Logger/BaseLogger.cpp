#include "Misc/StringTools.h"
#include "Logger/BaseLogger.h"

/*
==================
CBaseLogger::Printf
==================
*/
void CBaseLogger::Printf( ELogType InLogType, const tchar* InMessage, ... )
{
#if !NO_LOGGING
	va_list			arguments;
	va_start( arguments, InMessage );
    Serialize( L_Vsprintf( InMessage, arguments ).c_str(), InLogType );  
	va_end( arguments );
#endif // !NO_LOGGING
}