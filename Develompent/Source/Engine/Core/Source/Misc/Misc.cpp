#include <zlib.h>

#include "Logger/LoggerMacros.h"
#include "Misc/Misc.h"
#include "System/Archive.h"

/*
==================
Sys_CompressMemoryZLIB
==================
*/
static bool Sys_CompressMemoryZLIB( void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long		zCompressedSize = InOutCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Compress data
	bool		operationSucceeded = compress( ( byte* )InCompressedBuffer, &zCompressedSize, ( const byte* )InUncompressedBuffer, zUncompressedSize ) == Z_OK ? TRUE : FALSE;
	
	// Propagate compressed size from intermediate variable back into out variable.
	InOutCompressedSize = zCompressedSize;
	return operationSucceeded;
}

/*
==================
Sys_UncompressMemoryZLIB
==================
*/
static bool Sys_UncompressMemoryZLIB( void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long		zCompressedSize = InCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Uncompress data.
	bool		operationSucceeded = uncompress( ( byte* )InUncompressedBuffer, &zUncompressedSize, ( const byte* )InCompressedBuffer, zCompressedSize ) == Z_OK ? TRUE : FALSE;

	// Sanity Assert to make sure we uncompressed as much data as we expected to.
	Assert( InUncompressedSize == zUncompressedSize );
	return operationSucceeded;
}

/*
==================
Sys_CompressMemory
==================
*/
bool Sys_CompressMemory( ECompressionFlags InFlags, void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Make sure a valid compression scheme was provided
	Assert( InFlags | CF_ZLIB );
	bool		compressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		compressSucceeded = Sys_CompressMemoryZLIB( InCompressedBuffer, InOutCompressedSize, InUncompressedBuffer, InUncompressedSize );
		break;

	default:
		Warnf( TEXT( "Sys_CompressMemory :: Compression flags 0x%X :: This compression type not supported\n" ), InFlags );
		compressSucceeded = false;
		break;
	}

	return compressSucceeded;
}

/*
==================
Sys_UncompressMemory
==================
*/
bool Sys_UncompressMemory( ECompressionFlags InFlags, void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Make sure a valid compression scheme was provided
	Assert( InFlags | CF_ZLIB );
	bool		uncompressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		uncompressSucceeded = Sys_UncompressMemoryZLIB( InUncompressedBuffer, InUncompressedSize, InCompressedBuffer, InCompressedSize );
		break;

	default:
		Warnf( TEXT( "Sys_UncompressMemory :: Compression flags 0x%X :: This compression type not supported\n" ), InFlags );
		uncompressSucceeded = false;
		break;
	}

	return uncompressSucceeded;
}