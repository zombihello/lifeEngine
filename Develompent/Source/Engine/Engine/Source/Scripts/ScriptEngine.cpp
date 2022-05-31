#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptEngine.h"

// ----------------
// STATIC VALUES
// ----------------

//std::vector< void(*)( struct lua_State* InVM ) >			CScriptEngine::registerClassAPIs;

/**
 * Initialize script engine
 */
void CScriptEngine::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "Lua version: %s" ), ANSI_TO_TCHAR( LUA_RELEASE ) );
	LE_LOG( LT_Log, LC_Init, TEXT( "LuaJIT version: %s" ), ANSI_TO_TCHAR( LUAJIT_VERSION ) );
}