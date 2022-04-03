#include "System/Archive.h"
#include "Misc/Template.h"
#include "LEVersion.h"

FArchive::FArchive( const std::wstring& InPath )
	: arVer( VER_PACKAGE_LATEST )
	, arType( AT_TextFile )
	, arPath( InPath )
{}

void FArchive::SerializeHeader()
{
	// Serialize header package
	uint32		archiveFileTag = ARCHIVE_FILE_TAG;
	*this << archiveFileTag;
	checkMsg( archiveFileTag == ARCHIVE_FILE_TAG, TEXT( "Unknown archive file tag. Current archive file tag is 0x%X, need 0x%X" ), archiveFileTag, ARCHIVE_FILE_TAG );

	*this << arVer;
	checkMsg( arVer >= VER_MinVersion, TEXT( "Min supported version %i, but current version is %i" ), VER_MinVersion, arVer );

	*this << arType;
}

void FArchive::SerializeCompressed( void* InBuffer, uint32 InSize, ECompressionFlags InFlags )
{
	if ( InFlags == CF_None )
	{
		Serialize( InBuffer, InSize );
		return;
	}

	if ( arVer >= VER_CompressedZlib && IsLoading() )
	{
		// Read in base summary
		FCompressedChunkInfo		summary;
		*this << summary;

		// Handle change in compression chunk size in backward compatible way
		uint32			loadingCompressionChunkSize = LOADING_COMPRESSION_CHUNK_SIZE;

		// Figure out how many chunks there are going to be based on uncompressed size and compression chunk size.
		uint32	totalChunkCount = ( summary.uncompressedSize + loadingCompressionChunkSize - 1 ) / loadingCompressionChunkSize;

		// Allocate compression chunk infos and serialize them, keeping track of max size of compression chunks used.
		FCompressedChunkInfo*	compressionChunks = new FCompressedChunkInfo[ totalChunkCount ];
		uint32					maxCompressedSize = 0;
		for ( uint32 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
		{
			*this << compressionChunks[ chunkIndex ];
			maxCompressedSize = Max( compressionChunks[ chunkIndex ].compressedSize, maxCompressedSize );
		}

		// Set up destination pointer and allocate memory for compressed chunk[s] (one at a time).
		byte*			dest = ( byte* )InBuffer;
		void*			compressedBuffer = malloc( maxCompressedSize );

		// Iterate over all chunks, serialize them into memory and decompress them directly into the destination pointer
		for ( uint32 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
		{
			const FCompressedChunkInfo&			chunk = compressionChunks[ chunkIndex ];
			
			// Read compressed data.
			Serialize( compressedBuffer, chunk.compressedSize );
			
			// Decompress into dest pointer directly.
			bool		result = appUncompressMemory( InFlags, dest, chunk.uncompressedSize, compressedBuffer, chunk.compressedSize );
			check( result );
			
			// And advance it by read amount.
			dest += chunk.uncompressedSize;
		}

		// Free up allocated memory.
		free( compressedBuffer );
		delete[] compressionChunks;
	}
	else if ( IsSaving() )
	{
		// Figure out how many chunks there are going to be based on uncompressed size and compression chunk size
		uint32			totalChunkCount = ( InSize + SAVING_COMPRESSION_CHUNK_SIZE - 1 ) / SAVING_COMPRESSION_CHUNK_SIZE + 1;

		// Keep track of current position so we can later seek back and overwrite stub compression chunk infos
		uint32			startPosition = Tell();

		// Allocate compression chunk infos and serialize them so we can later overwrite the data
		FCompressedChunkInfo*		compressionChunks = new FCompressedChunkInfo[ totalChunkCount ];
		for ( uint32 chunkIndex = 0; chunkIndex < totalChunkCount; ++chunkIndex )
		{
			*this << compressionChunks[ chunkIndex ];
		}
	
		compressionChunks[ 0 ].uncompressedSize = InSize;		// The uncompressd size is equal to the passed in length
		compressionChunks[ 0 ].compressedSize = 0;				// Zero initialize compressed size so we can update it during chunk compression

		// Set up source pointer amount of data to copy (in bytes)
		byte*		src = ( byte* )InBuffer;

		int32		bytesRemaining = InSize;		
		uint32		currentChunkIndex = 1;												// Start at index 1 as first chunk info is summary.
		uint32		compressedBufferSize = SAVING_COMPRESSION_CHUNK_SIZE * 2;			// 2 times the uncompressed size should be more than enough; the compressed data shouldn't be that much larger
		void*		compressedBuffer = malloc( compressedBufferSize );

		while ( bytesRemaining > 0 )
		{
			uint32		bytesToCompress = Min( bytesRemaining, SAVING_COMPRESSION_CHUNK_SIZE );
			uint32		compressedSize = compressedBufferSize;

			bool		result = appCompressMemory( InFlags, compressedBuffer, compressedSize, src, bytesToCompress );
			check( result );

			// Move to next chunk
			src += bytesToCompress;
			Serialize( compressedBuffer, compressedSize );

			// Keep track of total compressed size, stored in first chunk.
			compressionChunks[ 0 ].compressedSize += compressedSize;

			// Update current chunk.
			check( currentChunkIndex < totalChunkCount );
			compressionChunks[ currentChunkIndex ].compressedSize = compressedSize;
			compressionChunks[ currentChunkIndex ].uncompressedSize = bytesToCompress;
			currentChunkIndex++;

			bytesRemaining -= SAVING_COMPRESSION_CHUNK_SIZE;
		}

		// Free allocated memory.
		free( compressedBuffer );

		// Overrwrite chunk infos by seeking to the beginning, serializing the data and then
		// seeking back to the end.
		uint32			endPosition = Tell();
		
		// Seek to the beginning.
		Seek( startPosition );
		
		// Serialize chunk infos.
		for ( uint32 chunkIndex = 0; chunkIndex < totalChunkCount; chunkIndex++ )
		{
			*this << compressionChunks[ chunkIndex ];
		}

		// Seek back to end.
		Seek( endPosition );

		// Free intermediate data.
		delete[] compressionChunks;
	}
}