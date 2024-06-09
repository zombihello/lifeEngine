#include <memory>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>
}

#include <LuaBridge/LuaBridge.h>

#include "Reflection/Class.h"
#include "Logger/BaseLogger.h"

#include <string>
#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Misc/StringConv.h"

/*
==================
Print
==================
*/
void Print( std::string Instr )
{
	Logf( TEXT( "%s\n" ), ANSI_TO_TCHAR( Instr.c_str() ) );
}


BEGIN_SCRIPT_API( CBaseLogger )
	luabridge::getGlobalNamespace( InVM )
		.addFunction( "Log", &Print )
		.beginClass<CBaseLogger>("Logger")
		.endClass();
END_SCRIPT_API()

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