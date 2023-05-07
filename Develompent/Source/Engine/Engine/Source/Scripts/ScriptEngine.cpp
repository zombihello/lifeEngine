#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptEngine.h"

// ----------------
// STATIC VALUES
// ----------------

//std::vector< void(*)( struct lua_State* InVM ) >			CScriptEngine::registerClassAPIs;

/*
==================
CScriptEngine::Init
==================
*/
void CScriptEngine::Init()
{
	Logf( TEXT( "Lua version: %s\n" ), ANSI_TO_TCHAR( LUA_RELEASE ) );
	Logf( TEXT( "LuaJIT version: %s\n" ), ANSI_TO_TCHAR( LUAJIT_VERSION ) );
}