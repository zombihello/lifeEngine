/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <zlib.h>

#include "Core/Logger/LoggerMacros.h"
#include "Core/Misc/Misc.h"
#include "Core/System/Archive.h"

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