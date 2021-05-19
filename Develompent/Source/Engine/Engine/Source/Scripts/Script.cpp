#include "System/BaseArchive.h"
#include "Misc/Class.h"
#include "Scripts/Script.h"

/**
 * Constructor
 */
FScript::FScript() : 
	luaVM( luaL_newstate() )
{
	check( luaVM );

	// Create new Lua VM and open libs
	luabridge::enableExceptions( luaVM );
	luaJIT_setmode( luaVM, 0, LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON );
	luaL_openlibs( luaVM );

	// Register of classes API in Lua
	FScriptEngine::StaticRegisterClassAPI( luaVM );
}

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
	if ( InArchive.IsLoading() )
	{
		EArchiveResourceType		resourceType;
		InArchive << resourceType;

		// If resource type not valid - this regular file and we try load source code of script
		if ( resourceType > ART_Count )
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
			SetByteCode( buffer, archiveSize );
			delete[] buffer;
		}

		// Else if resourceType is ART_Script - this is section of script
		else if ( resourceType == ART_Script )
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
		else
		{
			appErrorf( TEXT( "The current section in the archive is not a script" ) );
			return;
		}
	}
	else
	{
		// Write type of resource
		InArchive << ART_Script;

		// Write name
		InArchive << ( uint32 )name.size();
		InArchive.Serialize( ( byte* )name.data(), ( uint32 )name.size() );

		// Write byte code
		InArchive << ( uint32 )byteCode.size();
		InArchive.Serialize( byteCode.data(), ( uint32 )byteCode.size() );
	}
}

/**
 * Set byte code
 */
void FScript::SetByteCode( const byte* InByteCode, uint32 InSize )
{
	check( InByteCode && InSize > 0 );
	
	// loading byte code of script
	int32		result = luaL_loadbufferx( luaVM, ( achar* )InByteCode, InSize, name.c_str(), "bt" );		// b - binary format, t - text format, bt - all
	check( result == 0 );

	// Initialize script with help first call
	result = lua_pcall( luaVM, 0, 1, 0 );
	check( result == 0 );

	// Save byte code for serialization
	byteCode.resize( InSize );
	memcpy( byteCode.data(), InByteCode, InSize );
}