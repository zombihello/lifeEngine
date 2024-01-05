#include "System/Archive.h"
#include "Misc/Template.h"
#include "Reflection/Object.h"
#include "Reflection/Linker.h"
#include "Reflection/ObjectPackage.h"
#include "LEVersion.h"

/*
==================
CArchive::CArchive
==================
*/
CArchive::CArchive( const std::wstring& InPath )
	: arVer( VER_PACKAGE_LATEST )
	, arType( AT_TextFile )
	, arPath( InPath )
{}

/*
==================
CArchive::SerializeHeader
==================
*/
void CArchive::SerializeHeader()
{
	// Serialize header package
	uint32		archiveFileTag = ARCHIVE_FILE_TAG;
	*this << archiveFileTag;
	AssertMsg( archiveFileTag == ARCHIVE_FILE_TAG, TEXT( "Unknown archive file tag. Current archive file tag is 0x%X, need 0x%X" ), archiveFileTag, ARCHIVE_FILE_TAG );

	*this << arVer;
	AssertMsg( arVer >= VER_MinVersion, TEXT( "Min supported version %i, but current version is %i" ), VER_MinVersion, arVer );

	*this << arType;
}

/*
==================
CArchive::SerializeCompressed
==================
*/
void CArchive::SerializeCompressed( void* InBuffer, uint32 InSize, ECompressionFlags InFlags )
{
	if ( InFlags == CF_None )
	{
		Serialize( InBuffer, InSize );
		return;
	}

	if ( arVer >= VER_CompressedZlib && IsLoading() )
	{
		// Read in base summary
		CompressedChunkInfo		summary;
		*this << summary;

		// Handle change in compression chunk size in backward compatible way
		uint32			loadingCompressionChunkSize = LOADING_COMPRESSION_CHUNK_SIZE;

		// Figure out how many chunks there are going to be based on uncompressed size and compression chunk size.
		uint32	totalChunkCount = ( summary.uncompressedSize + loadingCompressionChunkSize - 1 ) / loadingCompressionChunkSize;

		// Allocate compression chunk infos and serialize them, keeping track of max size of compression chunks used.
		CompressedChunkInfo*	compressionChunks = new CompressedChunkInfo[ totalChunkCount ];
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
			const CompressedChunkInfo&			chunk = compressionChunks[ chunkIndex ];
			
			// Read compressed data.
			Serialize( compressedBuffer, chunk.compressedSize );
			
			// Decompress into dest pointer directly.
			bool		result = Sys_UncompressMemory( InFlags, dest, chunk.uncompressedSize, compressedBuffer, chunk.compressedSize );
			Assert( result );
			
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
		CompressedChunkInfo*		compressionChunks = new CompressedChunkInfo[ totalChunkCount ];
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

			bool		result = Sys_CompressMemory( InFlags, compressedBuffer, compressedSize, src, bytesToCompress );
			Assert( result );

			// Move to next chunk
			src += bytesToCompress;
			Serialize( compressedBuffer, compressedSize );

			// Keep track of total compressed size, stored in first chunk.
			compressionChunks[ 0 ].compressedSize += compressedSize;

			// Update current chunk.
			Assert( currentChunkIndex < totalChunkCount );
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


/*
==================
CArchiveSaveTagExports::CArchiveSaveTagExports
==================
*/
CArchiveSaveTagExports::CArchiveSaveTagExports( CObject* InOuter )
	: CArchive( InOuter ? CString::Format( TEXT( "SaveTagExports (%s)" ), InOuter->GetName().c_str() ) : TEXT( "SaveTagExports" ) )
	, outer( InOuter )
{}

/*
==================
CArchiveSaveTagExports::IsSaving
==================
*/
bool CArchiveSaveTagExports::IsSaving() const
{
	return true;
}

/*
==================
CArchiveSaveTagExports::operator<<
==================
*/
CArchive& CArchiveSaveTagExports::operator<<( class CObject*& InValue )
{
	if ( InValue && IsIn( InValue, outer ) && !InValue->HasAnyObjectFlags( OBJECT_TagExp ) )
	{
		// Set flags
		InValue->AddObjectFlag( OBJECT_TagExp );

		// Recurse with this object's class and package
		CObject*	theClass = ( CObject* )InValue->GetClass();
		CObject*	parent = InValue->GetOuter();
		*this << theClass << parent;

		// Add the object to array with tagged objects
		taggedObjects.push_back( InValue );
	}

	return *this;
}

/*
==================
CArchiveSaveTagExports::ProcessBaseObject
==================
*/
void CArchiveSaveTagExports::ProcessBaseObject( class CObject* InBaseObject )
{
	( *this ) << InBaseObject;
	ProcessTaggedObjects();
}

/*
==================
CArchiveSaveTagExports::ProcessTaggedObjects
==================
*/
void CArchiveSaveTagExports::ProcessTaggedObjects()
{
	std::vector<CObject*>		currentlyTaggedObjects;
	while ( !taggedObjects.empty() )
	{
		currentlyTaggedObjects.insert( currentlyTaggedObjects.end(), taggedObjects.begin(), taggedObjects.end() );
		taggedObjects.clear();

		for ( uint32 objIndex = 0, objCount = currentlyTaggedObjects.size(); objIndex < objCount; ++objIndex )
		{
			// Recurse with this object's children
			CObject*	object = currentlyTaggedObjects[objIndex];
			object->Serialize( *this );
		}

		currentlyTaggedObjects.clear();
	}
}


/*
==================
CArchiveSaveTagExports::CArchiveSaveTagImports
==================
*/
CArchiveSaveTagImports::CArchiveSaveTagImports( CLinkerSave* InLinker )
	: CArchive( InLinker && InLinker->GetLinkerRoot() ? CString::Format( TEXT( "SaveTagImports (%s)" ), InLinker->GetLinkerRoot()->GetName().c_str() ) : TEXT( "SaveTagImports" ) )
	, linker( InLinker )
{}

/*
==================
CArchiveSaveTagImports::IsSaving
==================
*/
bool CArchiveSaveTagImports::IsSaving() const
{
	return true;
}

/*
==================
CArchiveSaveTagExports::operator<<
==================
*/
CArchive& CArchiveSaveTagImports::operator<<( class CObject*& InValue )
{
	if ( InValue && !InValue->IsPendingKill() && ( !InValue->HasAnyObjectFlags( OBJECT_Transient ) || InValue->HasAnyObjectFlags( OBJECT_Native ) ) )
	{
		if ( !InValue->HasAnyObjectFlags( OBJECT_TagExp ) )
		{
			// Mark this object as an import
			InValue->AddObjectFlag( OBJECT_TagImp );

			// Look at parent of this object
			CObject*	outer = InValue->GetOuter();
			if ( outer )
			{
				*this << outer;
			}
		}
	}

	return *this;
}