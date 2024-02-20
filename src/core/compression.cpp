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

#include "pch_core.h"
#include <zlib.h>

#include "core/globals.h"
#include "core/debug.h"
#include "core/compression.h"

/*
==================
CompressMemoryZLIB
==================
*/
static bool CompressMemoryZLIB( void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long	zCompressedSize = compressedSize;
	unsigned long	zUncompressedSize = uncompressedSize;

	// Compress data
	bool bResult = compress( ( byte* )pCompressedBuffer, &zCompressedSize, ( const byte* )pUncompressedBuffer, zUncompressedSize ) == Z_OK ? TRUE : FALSE;

	// Propagate compressed size from intermediate variable back into out variable.
	compressedSize = zCompressedSize;
	return bResult;
}

/*
==================
UncompressMemoryZLIB
==================
*/
static bool UncompressMemoryZLIB( void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long	zCompressedSize = compressedSize;
	unsigned long	zUncompressedSize = uncompressedSize;

	// Uncompress data.
	bool bResult = uncompress( ( byte* )pUncompressedBuffer, &zUncompressedSize, ( const byte* )pCompressedBuffer, zCompressedSize ) == Z_OK ? TRUE : FALSE;

	// Sanity check to make sure we uncompressed as much data as we expected to.
	Assert( uncompressedSize == zUncompressedSize );
	return bResult;
}

/*
==================
Sys_CompressMemory
==================
*/
bool Sys_CompressMemory( ECompressionFlags flags, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize )
{
	// Make sure a valid compression scheme was provided
	Assert( flags | COMPRESSION_ZLIB );
	bool	bResult = false;

	switch ( flags )
	{
	case COMPRESSION_ZLIB:
		bResult = CompressMemoryZLIB( pCompressedBuffer, compressedSize, pUncompressedBuffer, uncompressedSize );
		break;

	default:
		Warning( "Sys_CompressMemory: Compression flags 0x%X, this compression type not supported", flags );
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
bool Sys_UncompressMemory( ECompressionFlags flags, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize )
{
	// Make sure a valid compression scheme was provided
	Assert( flags | COMPRESSION_ZLIB );
	bool	bResult = false;

	switch ( flags )
	{
	case COMPRESSION_ZLIB:
		bResult = UncompressMemoryZLIB( pUncompressedBuffer, uncompressedSize, pCompressedBuffer, compressedSize );
		break;

	default:
		Warning( "Sys_UncompressMemory: Compression flags 0x%X, this compression type not supported", flags );
		bResult = false;
		break;
	}

	return bResult;
}