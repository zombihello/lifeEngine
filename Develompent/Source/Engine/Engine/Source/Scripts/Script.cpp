#include "System/Archive.h"
#include "Reflection/Class.h"
#include "Scripts/Script.h"

/*
==================
CScript::CScript
==================
*/
CScript::CScript() : 
	CAsset( AT_Script ),
	luaVM( luaL_newstate() )
{
	Assert( luaVM );

	// Create new Lua VM and open libs
	luabridge::enableExceptions( luaVM );
	luaJIT_setmode( luaVM, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON );
	luaL_openlibs( luaVM );

	// Register of classes API in Lua
	CScriptEngine::StaticRegisterClassAPI( luaVM );
}

/*
==================
CScript::~CScript
==================
*/
CScript::~CScript()
{
	if ( luaVM )
	{
		lua_close( luaVM );
		luaVM = nullptr;
	}
}

/*
==================
CScript::Serialize
==================
*/
void CScript::Serialize( CArchive& InArchive )
{
	if ( InArchive.Type() != AT_TextFile )
	{
		CAsset::Serialize( InArchive );
	}

	if ( InArchive.IsLoading() )
	{
		if ( InArchive.Type() == AT_TextFile )
		{
			// Go to start of file
			InArchive.Seek( 0 );

			// Create string buffer and fill '\0'
			uint32				archiveSize = InArchive.GetSize() + 1;
			byte*				buffer = new byte[ archiveSize ];
			memset( buffer, '\0', archiveSize );

			// Serialize data to string buffer
			InArchive.Serialize( buffer, archiveSize );

			// Loading source code of script
			SetByteCode( buffer, strlen( ( achar* )buffer ) );
			delete[] buffer;
		}
		else
		{
			// Read name of script
			uint32			nameSize = 0;
			InArchive << nameSize;
			
			name.resize( nameSize );
			InArchive.Serialize( ( byte* )name.data(), nameSize );

			// Read byte code
			uint32			byteCodeSize = 0;
			InArchive << byteCodeSize;

			byte*			buffer = new byte[ byteCodeSize ];
			InArchive.Serialize( buffer, byteCodeSize );

			SetByteCode( buffer, byteCodeSize );
			delete[] buffer;
		}
	}
	else
	{
		// Write name
		InArchive << ( uint32 )name.size();
		InArchive.Serialize( ( byte* )name.data(), ( uint32 )name.size() );

		// Write byte code
		InArchive << ( uint32 )byteCode.size();
		InArchive.Serialize( byteCode.data(), ( uint32 )byteCode.size() );
	}
}

/*
==================
CScript::SetByteCode
==================
*/
void CScript::SetByteCode( const byte* InByteCode, uint32 InSize )
{
	Assert( InByteCode && InSize > 0 );

	// Loading byte code of script
	int32		result = luaL_loadbufferx( luaVM, ( achar* )InByteCode, InSize, name.c_str(), "bt" );		// b - binary format, t - text format, bt - all
	Assert( result == 0 );

	// Initialize script with help first call
	result = lua_pcall( luaVM, 0, 1, 0 );
	Assert( result == 0 );

	// Save byte code for serialization
	byteCode.resize( InSize );
	memcpy( byteCode.data(), InByteCode, InSize );
}