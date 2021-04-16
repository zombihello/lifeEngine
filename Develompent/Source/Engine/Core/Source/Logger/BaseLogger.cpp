#include <memory>

#include "Containers/String.h"
#include "Logger/BaseLogger.h"

/**
 * Print message to output device
 */
void BaseLogger::Logf( ELogType InLogType, ELogCategory InLogCategory, const tchar* InMessage, ... )
{
	va_list			arguments;
	va_start( arguments, InMessage );
	
	tchar*		buffer = String::Format( InMessage, arguments );
    Serialize( buffer, InLogType, InLogCategory );
   
	va_end( arguments );
	free( buffer );
}

