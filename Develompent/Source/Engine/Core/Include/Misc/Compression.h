/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief Size of chunk for loading compression data
 */
#define LOADING_COMPRESSION_CHUNK_SIZE			131072

/**
 * @ingroup Core
 * @brief Size of chunk for saving compression data
 */
#define SAVING_COMPRESSION_CHUNK_SIZE			LOADING_COMPRESSION_CHUNK_SIZE

/**
 * @ingroup Core
 * @brief Flags controlling compression
 */
enum ECompressionFlags
{
	CF_None = 0,		/**< No compression */
	CF_ZLIB = 1 << 0,	/**< Compress with ZLIB */
};

/**
 * @ingroup Core
 * @brief Helper structure for compression support, containing information on compressed
 * and uncompressed size of a chunk of data.
 */
struct CompressedChunkInfo
{
	uint32		compressedSize;			/**< Compressed size of data */
	uint32		uncompressedSize;		/**< Uncompresses size of data */
};

/**
 * @ingroup Core
 * @brief Compress data in memory
 * Thread-safe abstract compression routine. Compresses memory from uncompressed buffer and writes it to compressed
 * buffer. Updates CompressedSize with size of compressed data. Compression controlled by the passed in flags
 *
 * @param InFlags					Flags to control what method to use and optionally control memory vs speed
 * @param InCompressedBuffer		Buffer compressed data is going to be written to
 * @param OutCompressedSize			Size of InCompressedBuffer, at exit will be size of compressed data
 * @param InUncompressedBuffer		Buffer containing uncompressed data
 * @param InUncompressedSize		Size of uncompressed data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
bool Sys_CompressMemory( ECompressionFlags InFlags, void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize );

/**
 * @ingroup Core
 * @brief Uncompress data in memory 
 * Thread-safe abstract decompression routine. Uncompresses memory from compressed buffer and writes it to uncompressed
 * buffer. UncompressedSize is expected to be the exact size of the data after decompression
 *
 * @param InFlags					Flags to control what method to use to decompress
 * @param InUncompressedBuffer		Buffer containing uncompressed data
 * @param InUncompressedSize		Size of uncompressed data in bytes
 * @param InCompressedBuffer		Buffer compressed data is going to be read from
 * @param InCompressedSize			Size of InCompressedBuffer data in bytes
 * @return Return TRUE if compression succeeds, FALSE if it fails because pCompressedBuffer was too small or other reasons
 */
bool Sys_UncompressMemory( ECompressionFlags InFlags, void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize );

#endif // !COMPRESSION_H