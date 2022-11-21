#include <memory>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>
}

#include <LuaBridge/LuaBridge.h>

#include "Containers/String.h"
#include "Misc/Class.h"
#include "Logger/BaseLogger.h"

#include <string>
#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Containers/StringConv.h"

void Print( std::string Instr )
{
	LE_LOG( LT_Log, LC_Script, TEXT( "%s" ), ANSI_TO_TCHAR( Instr.c_str() ) );
}

IMPLEMENT_SCRIPT_API( CBaseLogger,
					  luabridge::getGlobalNamespace( InVM ).addFunction( "Log", &Print );
					  )

/**
 * Print message to output device
 */
void CBaseLogger::Logf( ELogType InLogType, ELogCategory InLogCategory, const tchar* InMessage, ... )
{
#if !NO_LOGGING
	va_list			arguments;
	va_start( arguments, InMessage );
    Serialize( CString::Format( InMessage, arguments ).c_str(), InLogType, InLogCategory );  
	va_end( arguments );
#endif // !NO_LOGGING
}