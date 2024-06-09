#include <zlib.h>

#include "Logger/LoggerMacros.h"
#include "Misc/Compression.h"

/*
==================
CompressMemoryZLIB
==================
*/
static bool CompressMemoryZLIB( void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Zlib wants to use unsigned long
	unsigned long		zCompressedSize = InOutCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Compress data
	bool bResult = compress( ( byte* )InCompressedBuffer, &zCompressedSize, ( const byte* )InUncompressedBuffer, zUncompressedSize ) == Z_OK ? true : false;
	
	// Propagate compressed size from intermediate variable back into out variable
	InOutCompressedSize = zCompressedSize;
	return bResult;
}

/*
==================
UncompressMemoryZLIB
==================
*/
static bool UncompressMemoryZLIB( void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Zlib wants to use unsigned long
	unsigned long		zCompressedSize = InCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Uncompress data
	bool bResult = uncompress( ( byte* )InUncompressedBuffer, &zUncompressedSize, ( const byte* )InCompressedBuffer, zCompressedSize ) == Z_OK ? true : false;

	// Sanity Assert to make sure we uncompressed as much data as we expected to
	Assert( InUncompressedSize == zUncompressedSize );
	return bResult;
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
	bool	bResult = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		bResult = CompressMemoryZLIB( InCompressedBuffer, InOutCompressedSize, InUncompressedBuffer, InUncompressedSize );
		break;

	default:
		Warnf( TEXT( "Sys_CompressMemory: Compression flags 0x%X, this compression type isn't supported\n" ), InFlags );
		bResult = false;
		break;
	}

	return bResult;
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
	bool	bResult = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		bResult = UncompressMemoryZLIB( InUncompressedBuffer, InUncompressedSize, InCompressedBuffer, InCompressedSize );
		break;

	default:
		Warnf( TEXT( "Sys_UncompressMemory: Compression flags 0x%X, this compression type isn't supported\n" ), InFlags );
		bResult = false;
		break;
	}

	return bResult;
}