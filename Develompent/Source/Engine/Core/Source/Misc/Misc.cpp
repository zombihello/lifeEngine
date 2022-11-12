#include <zlib.h>

#include "Logger/LoggerMacros.h"
#include "Misc/Misc.h"
#include "System/Archive.h"

static bool appCompressMemoryZLIB( void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
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

static bool appUncompressMemoryZLIB( void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long		zCompressedSize = InCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Uncompress data.
	bool		operationSucceeded = uncompress( ( byte* )InUncompressedBuffer, &zUncompressedSize, ( const byte* )InCompressedBuffer, zCompressedSize ) == Z_OK ? TRUE : FALSE;

	// Sanity check to make sure we uncompressed as much data as we expected to.
	check( InUncompressedSize == zUncompressedSize );
	return operationSucceeded;
}

bool appCompressMemory( ECompressionFlags InFlags, void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Make sure a valid compression scheme was provided
	check( InFlags | CF_ZLIB );
	bool		compressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		compressSucceeded = appCompressMemoryZLIB( InCompressedBuffer, InOutCompressedSize, InUncompressedBuffer, InUncompressedSize );
		break;

	default:
		LE_LOG( LT_Warning, LC_General, TEXT( "appCompressMemory :: Compression flags 0x%X :: This compression type not supported" ), InFlags );
		compressSucceeded = false;
		break;
	}

	return compressSucceeded;
}

bool appUncompressMemory( ECompressionFlags InFlags, void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Make sure a valid compression scheme was provided
	check( InFlags | CF_ZLIB );
	bool		uncompressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		uncompressSucceeded = appUncompressMemoryZLIB( InUncompressedBuffer, InUncompressedSize, InCompressedBuffer, InCompressedSize );
		break;

	default:
		LE_LOG( LT_Warning, LC_General, TEXT( "appUncompressMemory :: Compression flags 0x%X :: This compression type not supported" ), InFlags );
		uncompressSucceeded = false;
		break;
	}

	return uncompressSucceeded;
}