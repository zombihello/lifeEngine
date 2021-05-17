extern "C"
{
	#include <lua.h>
	#include <lauxlib.h>
	#include <lualib.h>
	#include <luajit.h>
}

#include <LuaBridge/LuaBridge.h>

#include "System/BaseArchive.h"
#include "Misc/Class.h"
#include "Scripts/Script.h"

/**
 * Constructor
 */
FScript::FScript() : 
	luaVM( nullptr )
{}

/**
 * Destructor
 */
FScript::~FScript()
{
	if ( luaVM )
	{
		lua_close( luaVM );
		luaVM = nullptr;
	}
}

/**
 * Serialize script
 */
void FScript::Serialize( FBaseArchive& InArchive )
{
	if ( InArchive.IsSaving() )
	{
		appErrorf( TEXT( "FScript not supported saving in archive, only loading scripts" ) );
		return;
	}

	// Create new lua VM and open libs
	luaVM = luaL_newstate();
	luabridge::enableExceptions( luaVM );
	luaJIT_setmode( luaVM, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON );
	luaL_openlibs( luaVM );

	// Register in lua API
	FScriptEngine::StaticRegisterClassAPI( luaVM );

	// Create string buffer and fill '\0'
	uint32				archiveSize = InArchive.GetSize() + 1;
	byte*				buffer = new byte[ archiveSize ];
	memset( buffer, '\0', archiveSize );

	// Serialize data to string buffer
	InArchive.Serialize( buffer, archiveSize );
	
	int32		result = luaL_loadstring( luaVM, ( const char* )buffer );
	check( result == 0 );
	delete[] buffer;

	result = lua_pcall( luaVM, 0, 1, 0 );
	check( result == 0 );
}