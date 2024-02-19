/**
 * @file
 * @addtogroup core core
 *
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

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "core/coreapi.h"
#include "core/types.h"

/**
 * @ingroup core
 * @brief Flags controlling compression
 */
enum ECompressionFlags
{
	COMPRESSION_NONE	= 0,		/**< No compression */
	COMPRESSION_ZLIB	= 1 << 0,	/**< Compress with ZLIB */
};

/**
 * @ingroup core
 * @brief Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
 * buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags
 *
 * @param flags					Flags to control what method to use and optionally control memory vs speed
 * @param pCompressedBuffer		Buffer compressed data is going to be written to
 * @param compressedSize		Size of pCompressedBuffer, at exit will be size of compressed data
 * @param pUncompressedBuffer	Buffer containing uncompressed data
 * @param uncompressedSize		Size of uncompressed data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
CORE_INTERFACE bool Sys_CompressMemory( ECompressionFlags flags, void* pCompressedBuffer, uint32& compressedSize, const void* pUncompressedBuffer, uint32 uncompressedSize );

/**
 * @ingroup core
 * @brief Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
 * buffer. UncompressedSize is expected to be the exact size of the data after decompression
 *
 * @param flags					Flags to control what method to use to decompress
 * @param pUncompressedBuffer	Buffer containing uncompressed data
 * @param uncompressedSize		Size of uncompressed data in bytes
 * @param pCompressedBuffer		Buffer compressed data is going to be read from
 * @param compressedSize		Size of pCompressedBuffer data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
CORE_INTERFACE bool Sys_UncompressMemory( ECompressionFlags flags, void* pUncompressedBuffer, uint32 uncompressedSize, const void* pCompressedBuffer, uint32 compressedSize );

#endif // !COMPRESSION_H