#include <vector>
#include <unordered_set>
#include <algorithm>

#include "Misc/CoreGlobals.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/Class.h"
#include "Reflection/LinkerSave.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/LinkerManager.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/ObjectGlobals.h"
#include "System/BaseFileSystem.h"
#include "System/PackageFileCache.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CObjectPackage )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObjectPackage )

// Max merged compression chunk size (used by CFileCompressionHelper)
#define MAX_MERGED_COMPRESSION_CHUNKSIZE		1024 * 1024

// Min merged compression chunk size (used by CFileCompressionHelper)
#define MIN_MERGED_COMPRESSION_CHUNKSIZE		384 * 1024

//
// GLOBALS
//
bool		CObjectPackage::bIsSavingPackage = false;

/**
 * @ingroup Core
 * @brief Helper class for mark referenced names to save
 */
class CPackageNameMapSaver
{
public:
	/**
	 * @brief Mark name as references
	 * @param InName	Name to mark
	 */
	FORCEINLINE void MarkNameAsReferenced( const CName& InName )
	{
		referencedNames.insert( InName.GetIndex() );
	}

	/**
	 * @brief Update a linker's NamesMap and serialize it
	 * @param InLinker		Linker
	 */
	void UpdateLinker( CLinkerSave& InLinker )
	{
		std::vector<CName>&		nameMap = InLinker.GetNames();
		std::vector<uint32>&	nameIndices = InLinker.GetNameIndices();
		nameIndices.resize( CName::GetMaxNames() );

		InLinker.GetSummary().nameOffset = InLinker.Tell();
		for ( auto it = referencedNames.begin(), itEnd = referencedNames.end(); it != itEnd; ++it )
		{
			CName::NameEntry*	nameEntry = CName::GetEntry( *it );
			nameIndices[*it]	= nameMap.size();

			Assert( nameEntry );
			nameMap.push_back( CName( ( EName )*it ) );
			InLinker << *nameEntry;
		}
		InLinker.GetSummary().nameCount = nameMap.size();
	}

private:
	std::unordered_set<uint32>		referencedNames;	/**< Set of references names */
};

/**
 * @ingroup Core
 * @brief Archive for tagging objects that must be exported to the file
 * It tags the objects passed to it, and recursively tags all of the objects this object references
 */
class CArchiveSaveTagExports : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * @param InOuter	The package to save
	 */
	CArchiveSaveTagExports( class CObject* InOuter )
		: CArchive( InOuter ? L_Sprintf( TEXT( "SaveTagExports (%s)" ), InOuter->GetName().c_str() ) : TEXT( "SaveTagExports" ) )
		, outer( InOuter )
	{
		arIsObjectReferenceCollector = true;
	}

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override
	{
		return true;
	}

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override
	{
		if ( InValue && !InValue->IsPendingKill() && IsIn( InValue, outer ) && !InValue->HasAnyObjectFlags( OBJECT_Transient | OBJECT_TagExp ) )
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

	/**
	 * @brief Serializes the specified object, tagging all objects it references
	 * @param InObject		The object that should be serialized, usually the package root
	 */
	void ProcessObject( class CObject* InObject )
	{
		*this << InObject;
		ProcessTaggedObjects();
	}

private:
	/**
	 * @brief Process tagged objects
	 * 
	 * Iterates over all objects which were encountered during serialization of the root object, serializing each one in turn.
	 * Objects encountered during that serialization are then added to the array and iteration continues until no new objects are
	 * added to the array
	 */
	void ProcessTaggedObjects()
	{
		std::vector<CObject*>		currentlyTaggedObjects;
		while ( !taggedObjects.empty() )
		{
			currentlyTaggedObjects.insert( currentlyTaggedObjects.end(), taggedObjects.begin(), taggedObjects.end() );
			taggedObjects.clear();

			for ( uint32 objIndex = 0, objCount = currentlyTaggedObjects.size(); objIndex < objCount; ++objIndex )
			{
				// Recurse with this object's children
				CObject* object = currentlyTaggedObjects[objIndex];
				object->Serialize( *this );
			}

			currentlyTaggedObjects.clear();
		}
	}

	std::vector<class CObject*>		taggedObjects;		/**< Tagged objects */
	class CObject*					outer;				/**< Package we're currently saving. Only objects contained within this package will be tagged for serialization */
};

/**
 * @ingroup Core
 * @brief Archive for tagging objects and names that must be listed in the file's imports table
 */
class CArchiveSaveTagImports : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * @param InLinker			The package linker to save
	 * @param InNameMapSaver	NameMap saver
	 */
	CArchiveSaveTagImports( class CLinkerSave* InLinker, CPackageNameMapSaver& InNameMapSaver )
		: CArchive( InLinker&& InLinker->GetLinkerRoot() ? L_Sprintf( TEXT( "SaveTagImports (%s)" ), InLinker->GetLinkerRoot()->GetName().c_str() ) : TEXT( "SaveTagImports" ) )
		, linker( InLinker )
		, nameMapSaver( InNameMapSaver )
	{
		arIsObjectReferenceCollector = true;
	}

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override
	{
		return true;
	}

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override
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

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CName& InValue ) override
	{
		nameMapSaver.MarkNameAsReferenced( InValue );
		return *this;
	}

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( const class CName& InValue ) override
	{
		nameMapSaver.MarkNameAsReferenced( InValue );
		return *this;
	}

private:
	class CLinkerSave*		linker;			/**< The package linker to save */
	CPackageNameMapSaver&	nameMapSaver;	/**< Name map saver */
};

/**
 * @ingroup Core
 * @brief Helper class for clarification, encapsulation, and elimination of duplicate code
 */
struct PackageExportTagger
{
	/**
	 * @brief Constructor
	 * 
	 * @param InBase			The object that should be saved into the package
	 * @param InTopLevelFlags	For all objects which are not referenced (either directly, or indirectly) through Base, only objects that contain any of these flags will be saved
	 * @param InOuter			The outer to use for the new package
	 */
	FORCEINLINE PackageExportTagger( CObject* InBase, ObjectFlags_t InTopLevelFlags, CObject* InOuter )
		: base( InBase )
		, topLevelFlags( InTopLevelFlags )
		, outer( InOuter )
	{}

	/**
	 * @brief Tag package exports
	 * 
	 * @param InExportTagger	Export tagger
	 * @param InIsRoutePreSave	Is need call CObject::PreSave()
	 */
	void TagPackageExports( CArchiveSaveTagExports& InExportTagger, bool InIsRoutePreSave )
	{
		// Route CObject::PreSave on Base and serialize it for export tagging
		if ( base )
		{
			if ( InIsRoutePreSave )
			{
				base->PreSave();
			}

			InExportTagger.ProcessObject( base );
		}

		// Serialize objects to tag them as OBJECT_TagExp
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( topLevelFlags ) || IsIn( object, outer ) )
			{
				InExportTagger.ProcessObject( object );
			}
		}

		// Route CObject::PreSave again
		if ( InIsRoutePreSave )
		{		
			for ( CObjectIterator it; it; ++it )
			{
				CObject*	object = *it;
				if ( object->HasAnyObjectFlags( OBJECT_TagExp ) )
				{
					object->PreSave();
				}
			}
		}
	}

	CObject*		base;			/**< The object that should be saved into the package */
	ObjectFlags_t	topLevelFlags;	/**< For all objects which are not referenced (either directly, or indirectly) through Base, only objects that contain any of these flags will be saved */
	CObject*		outer;			/**< The outer to use for the new package */
};

/**
 * @ingroup Core
 * @brief Helper class for package compression
 */
class CFileCompressionHelper
{
public:
	/**
	 * @brief Constructor
	 * @param InCompressionFlags	Flags to control compression (see ECompressionFlags)
	 */
	CFileCompressionHelper( uint32 InCompressionFlags )
		: compressionFlags( InCompressionFlags )
	{}

	/**
	 * @brief Compresses the passed in src file and writes it to destination
	 * 
	 * @param InSrcFilename		Filename to compress
	 * @param InDestFilename	Output name of compressed file, cannot be the same as InSrcFilename
	 * @param InSrcLinker		CLinkerSave object used to save src file
	 * @return Return TRUE if successful, otherwise FALSE
	 */
	bool CompressFile( const tchar* InSrcFilename, const tchar* InDestFilename, CLinkerSave& InSrcLinker )
	{
		Logf( TEXT( "Compressing package '%s'" ), InDestFilename );
		CFilename		tmpFilename = CFilename( InDestFilename ).GetPath() + CFilename( InDestFilename ).GetBaseFileName() + TEXT( "_Compressed.tmp" );

		// Create file reader and writer
		CArchive*		fileReader = g_FileSystem->CreateFileReader( InSrcFilename );
		CArchive*		fileWriter = g_FileSystem->CreateFileWriter( tmpFilename.GetFullPath().c_str() );

		// Abort if either operation wasn't successful
		if ( !fileReader || !fileWriter )
		{
			// Delete potentially created reader or writer
			delete fileReader;
			delete fileWriter;

			// Delete temporary file
			g_FileSystem->Delete( tmpFilename.GetFullPath().c_str() );
			return false;
		}

		// Compress archive
		CompressArchive( fileReader, fileWriter, InSrcLinker );

		// Delete file reader and writer
		delete fileReader;
		delete fileWriter;

		// Compression was successful, now move file
		bool bMoveSucceded = g_FileSystem->Move( InDestFilename, tmpFilename.GetFullPath().c_str(), true ) == CMR_OK;

		// Delete temporary file
		g_FileSystem->Delete( tmpFilename.GetFullPath().c_str() );
		return bMoveSucceded;
	}

private:
	/**
	 * @brief Compresses the passed in src archive and writes it to destination archive
	 * 
	 * @param InFileReader		Archive to read from
	 * @param InFileWriter		Archive to write to
	 * @param InSrcLinker		CLinkerSave object used to save src file
	 */
	void CompressArchive( CArchive* InFileReader, CArchive* InFileWriter, CLinkerSave& InSrcLinker )
	{
		// Read package file summary from source file
		PackageFileSummary		fileSummary;
		*InFileReader << fileSummary;

		// We don't compress the package file summary but treat everything afterwards
		// till the first export as a single chunk. This basically lumps name and import 
		// tables into one compressed block
		uint32		startOffset				= InFileReader->Tell();
		uint32		remainingHeaderSize		= InSrcLinker.GetSummary().totalHeaderSize - startOffset;
		currentChunk.uncompressedSize		= remainingHeaderSize;
		currentChunk.uncompressedOffset		= startOffset;

		// Iterate over all exports and add them separately. The underlying code will take
		// care of merging small blocks
		const std::vector<ObjectExport>&	exportMap = InSrcLinker.GetExports();
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			const ObjectExport&		exportObject = exportMap[index];
			AddToChunk( exportObject.serialSize );
		}

		// Finish chunk in flight and reset current chunk with size 0
		FinishCurrentAndCreateNewChunk( 0 );

		// Write base version of package file summary after updating compressed chunks array and compression flags
		fileSummary.compressionFlags = compressionFlags;
		fileSummary.compressedChunks = compressedChunks;
		*InFileWriter << fileSummary;

		// Reset internal state so subsequent calls will work
		compressedChunks.clear();
		currentChunk = CompressedChunk();

		// Allocate temporary buffers for reading and compression
		uint32		srcBufferSize	= remainingHeaderSize;
		void*		srcBuffer		= Memory::Malloc( srcBufferSize );

		// Iterate over all chunks, read the data, compress and write it out to destination file
		for ( uint32 chunkIndex = 0, numChunks = fileSummary.compressedChunks.size(); chunkIndex < numChunks; ++chunkIndex )
		{
			CompressedChunk&	chunk = fileSummary.compressedChunks[chunkIndex];

			// Increase temporary buffer sizes if they are too small
			if ( srcBufferSize < chunk.uncompressedSize )
			{
				srcBufferSize	= chunk.uncompressedSize;
				srcBuffer		= Memory::Realloc( srcBuffer, srcBufferSize );
			}

			// Verify that we're not skipping any data
			Assert( chunk.uncompressedOffset == InFileReader->Tell() );

			// Read src uncompressed data
			InFileReader->Serialize( srcBuffer, chunk.uncompressedSize );

			// Keep track of offset
			chunk.compressedOffset = InFileWriter->Tell();

			// Serialize compressed
			InFileWriter->SerializeCompressed( srcBuffer, chunk.uncompressedSize, ( ECompressionFlags )fileSummary.compressionFlags );

			// Keep track of compressed size
			chunk.compressedSize = InFileWriter->Tell() - chunk.compressedOffset;
		}

		// Verify that we've compressed everything
		Assert( InFileReader->IsEndOfFile() );

		// Serialize file summary again. At this time CompressedChunks array is going to contain compressed size and offsets
		InFileWriter->Seek( 0 );
		*InFileWriter << fileSummary;

		// Free intermediate buffers
		Memory::Free( srcBuffer );
	}

	/**
	 * @brief Tries to add bytes to current chunk and creates a new one if there is not enough space
	 * @param InSize	Number of bytes to try to add to current chunk
	 */
	void AddToChunk( uint32 InSize )
	{
		// Resulting chunk would be too big
		if ( currentChunk.uncompressedSize + InSize > MAX_MERGED_COMPRESSION_CHUNKSIZE && currentChunk.uncompressedSize > MIN_MERGED_COMPRESSION_CHUNKSIZE )
		{
			// Finish up current chunk and create a new one of passed in size
			FinishCurrentAndCreateNewChunk( InSize );
		}
		// Add bytes to existing chunk
		else
		{
			currentChunk.uncompressedSize += InSize;
		}
	}

	/**
	 * @brief Finish current chunk and add it to the CompressedChunks array. This also creates a new chunk with a base size passed in
	 * @param InSize	Size in bytes of new chunk to create
	 */
	void FinishCurrentAndCreateNewChunk( uint32 InSize )
	{
		if ( currentChunk.uncompressedSize > 0 )
		{
			compressedChunks.push_back( currentChunk );
			currentChunk					= CompressedChunk();
			currentChunk.uncompressedOffset = currentChunk.uncompressedOffset + currentChunk.uncompressedSize;
			currentChunk.uncompressedSize	= InSize;
		}
	}

	uint32								compressionFlags;		/**< Flags to control compression (see ECompressionFlags) */
	std::vector<CompressedChunk>		compressedChunks;		/**< Compressed chunks, populated by AddToChunk and FinishCurrentAndCreateNewChunk */
	CompressedChunk						currentChunk;			/**< Current chunk, used by merging code */
};

/*
==================
CObjectPackage::CObjectPackage
==================
*/
CObjectPackage::CObjectPackage()
	: bDirty( false )
	, bHasBeenFullyLoaded( false )
	, packageFlags( 0 )
	, linkerLoad( nullptr )
{}

/*
==================
CObjectPackage::CreatePackage
==================
*/
CObjectPackage* CObjectPackage::CreatePackage( CObject* InOuter, const tchar* InPackageName )
{
	std::wstring	finalName = InPackageName;
	
	// Make sure what package name is valid
	if ( !finalName.empty() && finalName[finalName.size()-1] == TEXT( '.' ) )
	{
		std::wstring	newName = finalName.substr( 0, finalName.size()-1 );
		Warnf( TEXT( "Invalid package name '%s', renamed to '%s'" ), finalName.c_str(), newName.c_str() );
		finalName = newName;
	}

	// If final name is empty make unique name
	if ( finalName.empty() )
	{
		finalName = CObject::MakeUniqueObjectName( InOuter, CObjectPackage::StaticClass() ).ToString();
	}

	// Resolve a package and name
	ResolvePackageAndName( InOuter, finalName, true );
	if ( finalName.empty() )
	{
		Sys_Error( TEXT( "CreatePackage: Can't create a package without name" ) );
	}

	// Find or create a package if finalName isn't None
	CObjectPackage*		result = nullptr;
	if ( finalName != TEXT( "None" ) )
	{
		result = FindObject<CObjectPackage>( InOuter, finalName.c_str() );
		if ( !result )
		{
			// Create a new package
			result = new( InOuter, finalName, OBJECT_Public ) CObjectPackage;
		
			// Mark this package as newly created
			result->AddPackageFlag( PKG_NewlyCreated );
		}
	}
	// Otherwise it's critical error
	else
	{
		Sys_Error( TEXT( "CreatePackage: Can't create a package with name 'None'" ) );
	}

	return result;
}

/*
==================
CObjectPackage::LoadPackage
==================
*/
CObjectPackage* CObjectPackage::LoadPackage( CObjectPackage* InOuter, const tchar* InFilename, uint32 InLoadFlags )
{
	CObjectPackage*		resultPackage = nullptr;
	Assert( *InFilename != '\0' );

	// Try to load
	Logf( TEXT( "Load package '%s'\n" ), InFilename );
	
	// We begin loading the package
	BeginLoadPackage();
	
	// Create a new linker object which goes off and tries load the file
	CLinkerLoad*	linker = CLinkerLoad::GetPackageLinker( InOuter, InFilename, InLoadFlags );
	if ( !linker )
	{
		EndLoadPackage();
		Errorf( TEXT( "Failed to load package '%s', no linker\n" ), InFilename );
		return nullptr;
	}
	resultPackage = linker->GetLinkerRoot();

	// Load all objects from package
	linker->LoadAllObjects();

	// Mark package as loaded
	resultPackage->AddObjectFlag( OBJECT_WasLoaded );

	// We end loading the package
	EndLoadPackage();

	// Done!
	Logf( TEXT( "Package '%s' is loaded\n" ), InFilename );
	return resultPackage;
}

/*
==================
CObjectPackage::BeginLoadPackage
==================
*/
void CObjectPackage::BeginLoadPackage()
{
	ObjectSerializeContext&		objectSerializeContext = GetObjectSerializeContext();
	if ( objectSerializeContext.IncrementBeginLoadCount() == 1 )
	{
		// Validate clean load state
		Assert( !objectSerializeContext.HasLoadedObjects() );
	}
}

/*
==================
CObjectPackage::EndLoadPackage
==================
*/
void CObjectPackage::EndLoadPackage()
{
	ObjectSerializeContext&		objectSerializeContext = GetObjectSerializeContext();
	while ( objectSerializeContext.DecrementBeginLoadCount() == 0 && ( objectSerializeContext.HasLoadedObjects() || objectSerializeContext.HasPendingImports() || objectSerializeContext.HasDelayedLinkerClosePackages() ) )
	{
		// Make sure we're not recursively calling EndLoadPackage
		objectSerializeContext.IncrementBeginLoadCount();

		// Temporary list of loaded objects as ObjectSerializeContext::objectsLoaded might expand during iteration
		std::vector<CObject*>				objLoaded;
		std::unordered_set<CLinkerLoad*>	loadedLinkers;
		while ( objectSerializeContext.HasLoadedObjects() )
		{
			// Accumulate till ObjectSerializeContext::objectsLoaded no longer increases
			objectSerializeContext.AppendLoadedObjectsAndEmpty( objLoaded );

			// Sort by Filename and Offset
			std::sort( objLoaded.begin(), objLoaded.end(), []( const CObject* InA, const CObject* InB ) -> bool 
					   { 
						   CLinker*		linkerA = InA->GetLinker();
						   CLinker*		linkerB = InB->GetLinker();

						   // Both objects have linkers
						   if ( linkerA && linkerB )
						   {
							   // Identical linkers, sort by offset in file
							   if ( linkerA == linkerB )
							   {
								   ObjectExport&	exportA = linkerA->GetExports()[ InA->GetLinkerIndex() ];
								   ObjectExport&	exportB = linkerB->GetExports()[ InB->GetLinkerIndex() ];
								   return exportA.serialOffset < exportB.serialOffset;
							   }
							   // Sort by pointer address
							   else
							   {
								   return false;
							   }
						   }
						   // Neither objects have a linker, don't do anything
						   else if ( linkerA == linkerB )
						   {
							   return false;
						   }
						   // Sort objects with linkers vs objects without
						   else
						   {
							   return linkerA != nullptr;
						   }
					   } );

			// Finish loading everything
			for ( uint32 index = 0, count = objLoaded.size(); index < count; ++index )
			{
				CObject*	object = objLoaded[index];
				if ( object->HasAnyObjectFlags( OBJECT_NeedLoad ) )
				{
					CLinkerLoad*	linker = object->GetLinker();
					Assert( linker );
					linker->Preload( object );
				}
			}

			// Start over again as new objects have been loaded that need to have "Preload" called on them before
			// we can safely PostLoad them
			if ( objectSerializeContext.HasLoadedObjects() )
			{
				continue;
			}

			// Remember linker of objects. Only in the editor
			if ( g_IsEditor )
			{
				for ( uint32 index = 0, count = objLoaded.size(); index < count; ++index )
				{
					CObject*		object = objLoaded[index];
					CLinkerLoad*	linker = object->GetLinker();
					if ( linker )
					{
						loadedLinkers.insert( linker );
					}
				}
			}

			// Call PostLoad of all loaded objects
			{
				TGuardValue<bool>	guardIsRoutingPostLoad( objectSerializeContext.bRoutingPostLoad, true );
				for ( uint32 index = 0, count = objLoaded.size(); index < count; ++index )
				{
					CObject*	object = objLoaded[index];
					Assert( object );
					object->ConditionalPostLoad();
				}
			}

			// Empty array before next iteration as we finished postloading all objects
			objLoaded.clear();
		}

		// Mark all loaded package as fully loaded if they all exports were loaded. Only in the editor
		if ( g_IsEditor && !loadedLinkers.empty() )
		{
			for ( auto it = loadedLinkers.begin(), itEnd = loadedLinkers.end(); it != itEnd; ++it )
			{
				CLinkerLoad*	linker = *it;
				Assert( linker );

				CObjectPackage* linkerRoot = linker->GetLinkerRoot();
				if ( linkerRoot && !linkerRoot->IsFullyLoaded() )
				{
					bool								bAllExportsCreated = true;
					const std::vector<ObjectExport>&	exportMap = linker->GetExports();
					for ( uint32 exportIndex = 0, exportCount = exportMap.size(); exportIndex < exportCount; ++exportIndex )
					{
						const ObjectExport&		exportObject = exportMap[exportIndex];
						if ( !exportObject.object )
						{
							bAllExportsCreated = false;
							break;
						}
					}

					if ( bAllExportsCreated )
					{
						linkerRoot->MarkAsFullyLoaded();
					}
				}
			}
		}

		// Dissociate all linker import, since they may be destroyed, causing their pointers to become invalid
		CLinkerManager::Get().DissociateImports();

		// Close any linkers' loaders that were requested to be closed once ObjectSerializeContext::objBeginLoadCount goes to 0
		std::unordered_set<CLinkerLoad*>		packagesToClose;
		GetObjectSerializeContext().GetDelayedLinkerClosePackagesAndEmpty( packagesToClose );
		for ( auto it = packagesToClose.begin(), itEnd = packagesToClose.end(); it != itEnd; ++it )
		{
			CLinkerLoad*	linker = *it;
			if ( linker )
			{
				CObjectPackage*		linkerRoot = linker->GetLinkerRoot();
				if ( linker->HasLoader() && linkerRoot )
				{
					CLinkerManager::Get().ResetLoaders( linkerRoot );
				}
				Assert( !linker->HasLoader() );
			}
		}
	}
}

/*
==================
CObjectPackage::SavePackage
==================
*/
bool CObjectPackage::SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename, uint32 InSaveFlags, uint32 InCompressionFlags /* = CF_None */, CBaseTargetPlatform* InCookingTarget /* = nullptr */ )
{
	// Check on recursive call SavePackage, it's error
	if ( bIsSavingPackage )
	{
		Warnf( TEXT( "Recursive CObjectPackage::SavePackage() is not supported\n" ) );
		AssertNoEntry();
		return false;
	}

	// Make temp file
	Assert( InOuter );
	double			timeStart = Sys_Seconds();
	std::wstring	baseFilename = CFilename( InFilename ).GetBaseFileName();
	CFilename		tempFilename = CFilename( InFilename ).GetPath() + PATH_SEPARATOR + baseFilename + TEXT( "_save.tmp" );
	bool			bSuccess = false;
	bool			bCleanupIsRequired = false;
	bool			bIsCooking = InCookingTarget != nullptr;
	bool			bFilterEditorOnly = InOuter->HasAnyPackageFlags( PKG_FilterEditorOnly ) || bIsCooking;

	// Don't save packages marked as editor-only in cooking
#if WITH_EDITOR
	if ( bIsCooking && InOuter->HasAnyPackageFlags( PKG_EditorOnly ) )
	{
		Warnf( TEXT( "Package marked as editor-only: %s. Package will not be saved\n" ), InOuter->GetName().c_str() );
		return false;
	}
#endif // WITH_EDITOR

	// Route CObject::PreSaveRoot to allow e.g. the world to attach components for the persistent level
	if ( InBase )
	{
		bCleanupIsRequired = InBase->PreSaveRoot( InFilename );
	}

	// Untag all objects
	for ( CObjectIterator it; it; ++it )
	{
		// Clear flags from previous SavePackage calls
		uint32		clearFlags = OBJECT_TagImp | OBJECT_TagExp;

		// Clear context flags for objects that are going to be saved into package
		it->RemoveObjectFlag( clearFlags );

		// If the object class is abstract or has been marked as deprecated, mark this
		// object as transient so that it isn't serialized
		if ( it->GetClass()->HasAnyClassFlags( CLASS_Abstract | CLASS_Deprecated ) )
		{
			if ( it->GetClass()->HasAnyClassFlags( CLASS_Deprecated ) )
			{
				it->AddObjectFlag( OBJECT_Transient );
			}

			if ( it->GetClass()->HasAnyClassFlags( CLASS_HasComponents ) )
			{
				std::vector<CActorComponent*>						componentReferences;
				TArchiveObjectReferenceCollector<CActorComponent>	componentCollector( &componentReferences, *it, false, true );
				it->Serialize( componentCollector );

				for ( uint32 index = 0, count = componentReferences.size(); index < count; ++index )
				{
					componentReferences[index]->AddObjectFlag( OBJECT_Transient );
				}
			}
		}
	}

	// Export objects (tags them as OBJECT_TagExp)
	CArchiveSaveTagExports		exportTaggerArchive( InOuter );
	exportTaggerArchive.SetFilterEditorOnly( bFilterEditorOnly );
	exportTaggerArchive.SetCookingTarget( InCookingTarget );

	// Tag exports and route CObject::PreSave
	PackageExportTagger			packageExportTagger( InBase, InTopLevelFlags, InOuter );
	packageExportTagger.TagPackageExports( exportTaggerArchive, true );

	{
		// Set CObjectPackage::bIsSavingPackage here as it is now illegal to create any new object references, they potentially wouldn't be saved correctly
		TGuardValue<bool>	isSavingFlag( bIsSavingPackage, true );

		// Clear OBJECT_TagExp again as we need to redo tagging below
		for ( CObjectIterator it; it; ++it )
		{
			it->RemoveObjectFlag( OBJECT_TagExp );
		}

		// We need to serialize objects yet again to tag objects that were created by CObject::PreSave as OBJECT_TagExp
		packageExportTagger.TagPackageExports( exportTaggerArchive, false );

		// Allocate a linker
		CLinkerSave				linker( InOuter, tempFilename.GetFullPath().c_str() );
		CPackageNameMapSaver	nameMapSaver;
		linker.SetFilterEditorOnly( bFilterEditorOnly );
		linker.SetCookingTarget( InCookingTarget );

		// Import objects and names
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( OBJECT_TagExp ) )
			{
				CArchiveSaveTagImports	importTagger( &linker, nameMapSaver );
				importTagger.SetFilterEditorOnly( bFilterEditorOnly );
				importTagger.SetCookingTarget( InCookingTarget );

				CClass*		theClass = object->GetClass();
				object->Serialize( importTagger );
				importTagger << theClass;

				// Object can be saved in package different than their outer, if our outer isn't the package being saved check if we need to tag it as import
				CObject*	outer = object->GetOuter();
				if ( outer->GetOutermost() != InOuter )
				{
					importTagger << outer;
				}
			}
		}

		// Tag the names for all relevant object, classes, and packages
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( OBJECT_TagExp | OBJECT_TagImp ) )
			{
				nameMapSaver.MarkNameAsReferenced( object->GetCName() );
				if ( object->GetOuter() )
				{
					nameMapSaver.MarkNameAsReferenced( object->GetOuter()->GetCName() );
				}

				if ( object->HasAnyObjectFlags( OBJECT_TagImp ) )
				{
					// Make sure the package name of an import is referenced as it might be different than its outer
					CObjectPackage*		objectPackage = object->GetOutermost();
					Assert( objectPackage );

					nameMapSaver.MarkNameAsReferenced( objectPackage->GetCName() );
					nameMapSaver.MarkNameAsReferenced( object->GetClass()->GetCName() );
					Assert( object->GetClass()->GetOuter() );
					nameMapSaver.MarkNameAsReferenced( object->GetClass()->GetOuter()->GetCName() );
				}
			}
		}

		// Save package flags
		{
			uint32		packageFlags = InOuter->GetPackageFlags();
			// Set flags PKG_Cooked | PKG_FilterEditorOnly if we save the package as cooked
			if ( bIsCooking )
			{
				packageFlags |= PKG_Cooked | PKG_FilterEditorOnly;
			}
			// Otherwise remove PKG_Cooked for make sure what this package will be right loaded
			// (because we can load a cooked package and save it without InCookingTarget)
			else
			{
				packageFlags &= ~PKG_Cooked;
			}

			// Set flag PKG_StoreCompressed if the package will be compressed
			if ( InCompressionFlags != CF_None )
			{
				packageFlags |= PKG_StoreCompressed;
			}

			// Save package flags
			linker.GetSummary().SetPackageFlags( packageFlags );
		}

		// Rest place for package summary, we update it in the end
		linker << linker.GetSummary();
		uint32		offsetAfterPackageFileSummary = linker.Tell();

		// Build and serialize name map
		nameMapSaver.UpdateLinker( linker );

		// Build import map
		std::vector<ObjectImport>&		importMap = linker.GetImports();
		for ( CObjectIterator it; it; ++it )
		{
			if ( it->HasAnyObjectFlags( OBJECT_TagImp ) )
			{
				importMap.push_back( *it );
			}
		}
		linker.GetSummary().importCount = importMap.size();

		// Build export map
		std::vector<ObjectExport>&		exportMap = linker.GetExports();
		for ( CObjectIterator it; it; ++it )
		{
			if ( it->HasAnyObjectFlags( OBJECT_TagExp ) )
			{
				exportMap.push_back( *it );
			}
		}
		linker.GetSummary().exportCount = exportMap.size();

		// Set linker reverse mappings
		std::vector<CPackageIndex>&		objectIndeces = linker.GetObjectIndices();
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			CObject*	object = exportMap[index].object;
			if ( object )
			{
				objectIndeces[object->GetIndex()] = CPackageIndex::FromExport( index );
			}
		}

		for ( uint32 index = 0, count = importMap.size(); index < count; ++index )
		{
			CObject*	object = importMap[index].object;
			if ( object )
			{
				objectIndeces[object->GetIndex()] = CPackageIndex::FromImport( index );
			}
		}

		// Save dummy import map, overwritten later
		linker.GetSummary().importOffset = linker.Tell();
		for ( uint32 index = 0, count = importMap.size(); index < count; ++index )
		{
			ObjectImport&	objectImport = importMap[index];
			linker << objectImport;
		}
		uint32		offsetAfterImportMap = linker.Tell();

		// Save dummy export map, overwritten later
		linker.GetSummary().exportOffset = linker.Tell();
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			ObjectExport&	objectExport = exportMap[index];
			linker << objectExport;
		}
		uint32		offsetAfterExportMap = linker.Tell();

		// Save total the package's header size
		linker.GetSummary().totalHeaderSize = linker.Tell();

		// Save exports
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			ObjectExport&	objectExport = exportMap[index];
			if ( objectExport.object )
			{
				// Set class index
				if ( !IsA<CClass>( objectExport.object ) )
				{
					objectExport.classIndex = objectIndeces[objectExport.object->GetClass()->GetIndex()];
					AssertMsg( !objectExport.classIndex.IsNull(), TEXT( "Export %s class is not mapped when saving %s" ), *objectExport.object->GetFullName().c_str(), linker.GetLinkerRoot()->GetName().c_str() );
				}
				else
				{
					// This is a CClass object
					objectExport.classIndex = CPackageIndex();
				}

				// Set the parent index, if this export represents a CStruct-derived object
				if ( IsA<CStruct>( objectExport.object ) )
				{
					CStruct*	theStruct = ( CStruct* )objectExport.object;
					if ( theStruct->GetSuperStruct() )
					{
						objectExport.superIndex = objectIndeces[theStruct->GetSuperStruct()->GetIndex()];
						AssertMsg( !objectExport.superIndex.IsNull(),
								   TEXT( "Export Struct (%s) of type (%s) inheriting from (%s) of type (%s) has not mapped super struct" ), 
								   theStruct->GetPathName().c_str(),
								   theStruct->GetClass()->GetPathName().c_str(),
								   theStruct->GetSuperStruct()->GetPathName().c_str(),
								   theStruct->GetSuperStruct()->GetClass()->GetPathName().c_str() );
					}
					else
					{
						objectExport.superIndex = CPackageIndex();
					}
				}
				else
				{
					objectExport.superIndex = CPackageIndex();
				}

				// Set CPackageIndex for this export's Outer. 
				// If the export's Outer is the CObjectPackage corresponding to this package's LinkerRoot
				if ( objectExport.object->GetOuter() != InOuter )
				{
					Assert( objectExport.object->GetOuter() );
					objectExport.outerIndex = objectIndeces[objectExport.object->GetOuter()->GetIndex()];

					AssertMsg( IsIn( objectExport.object->GetOuter(), InOuter ),
							   TEXT( "Export Object (%s) Outer (%s) mismatch" ),
							   objectExport.object->GetPathName().c_str(),
							   objectExport.object->GetOuter()->GetPathName().c_str() );
					AssertMsg( !objectExport.outerIndex.IsImport(),
							   TEXT( "Export Object (%s) Outer (%s) is an Import" ),
							   objectExport.object->GetPathName().c_str(),
							   objectExport.object->GetOuter()->GetPathName().c_str() );
					AssertMsg( !objectExport.outerIndex.IsNull(),
							   TEXT( "Export Object (%s) Outer (%s) index is root package" ),
							   objectExport.object->GetPathName().c_str(),
							   objectExport.object->GetOuter()->GetPathName().c_str() );

				}
				else
				{
					// This export's Outer is the LinkerRoot for this package
					objectExport.outerIndex = CPackageIndex();
				}

				// Save the object data
				objectExport.serialOffset = linker.Tell();
				objectExport.object->Serialize( linker );
				objectExport.serialSize = linker.Tell() - objectExport.serialOffset;
			}
		}

		// Save the import map
		linker.Seek( linker.GetSummary().importOffset );
		for ( uint32 index = 0, count = importMap.size(); index < count; ++index )
		{
			ObjectImport&		objectImport = importMap[index];
			if ( objectImport.object )
			{
				// Set the outer index
				if ( objectImport.object->GetOuter() )
				{
					if ( IsIn( objectImport.object->GetOuter(), InOuter ) )
					{
						if ( !objectImport.object->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) )
						{
							Warnf( TEXT( "Bad object '%s'\n" ), objectImport.object->GetFullName().c_str() );
						}
						else
						{
							// If an object is marked OBJECT_Native|OBJECT_Transient, it is either an intrinsic class or
							// a property of an intrinsic class. Only properties of intrinsic classes will have
							// an Outer that passes the check for "IsIn( GetOuter, InOuter )" (thus ending up in this
							// block of code). Just verify that the Outer for this property is also marked OBJECT_Transient|OBJECT_Native
							Assert( objectImport.object->GetOuter()->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) );
						}
					}

					Assert( !IsIn( objectImport.object->GetOuter(), InOuter ) || objectImport.object->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) );
					objectImport.outerIndex = objectIndeces[objectImport.object->GetOuter()->GetIndex()];
				}
			}

			// Save it
			linker << objectImport;
		}
		Assert( linker.Tell() == offsetAfterImportMap );

		// Save the export map
		linker.Seek( linker.GetSummary().exportOffset );
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			linker << exportMap[index];
		}
		Assert( linker.Tell() == offsetAfterExportMap );

		// Update package summary
		linker.Seek( 0 );
		linker << linker.GetSummary();
		Assert( linker.Tell() == offsetAfterPackageFileSummary );
		bSuccess = true;

		// Detach saver in the linker
		linker.Detach();
		Logf( TEXT( "Save package '%s' took %f secs\n" ), InFilename, Sys_Seconds() - timeStart );

		if ( bSuccess )
		{
			// Compress the temporarily file to destination
			if ( linker.GetSummary().GetPackageFlags() & PKG_StoreCompressed )
			{
				Logf( TEXT( "Compressing '%s' to '%s'\n" ), tempFilename.GetFullPath().c_str(), InFilename );
				CFileCompressionHelper		compressionHelper( InCompressionFlags );
				bSuccess = compressionHelper.CompressFile( tempFilename.GetFullPath().c_str(), InFilename, linker );
			}
			// Move the temporary file
			else
			{
				Logf( TEXT( "Moving '%s' to '%s'\n" ), tempFilename.GetFullPath().c_str(), InFilename );
				bSuccess = g_FileSystem->Move( InFilename, tempFilename.GetFullPath(), true ) == CMR_OK;
			}

			if ( !bSuccess )
			{
				Warnf( TEXT( "Failed to save package '%s'\n" ), InFilename );
			}
			else
			{
				// Mark exports and the package as OBJECT_WasLoaded after they've been serialized
				// This is to ensue that newly created packages are properly marked as loaded 
				// (since they now exist on disk and in memory in the exact same state)
				for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
				{
					ObjectExport&	objectExport = exportMap[index];
					if ( objectExport.object )
					{
						objectExport.object->AddObjectFlag( OBJECT_WasLoaded );
					}
				}

				// And finally set the flag on the package itself
				InOuter->AddObjectFlag( OBJECT_WasLoaded );

				// Package has been save, so unmark NewlyCreated flag and unset dirty flag
				InOuter->RemovePackageFlag( PKG_NewlyCreated );

				// Remember path to the package file
				InOuter->SetPackagePath( InFilename );

				// Clear dirty flag if desired
				if ( !( InSaveFlags & SAVE_KeepDirty ) )
				{
					InOuter->SetDirtyFlag( false );
				}

				// Update package file cache
				CPackageFileCache::Get().CachePackage( InFilename );
			}

			// Delete the temporary file
			g_FileSystem->Delete( tempFilename.GetFullPath() );
		}

		// Route CObject::PostSaveRoot to allow e.g. the world to detach components for the persistent level that where
		// attached in CObject::PreSaveRoot
		if ( InBase )
		{
			InBase->PostSaveRoot( bCleanupIsRequired );
		}
	}

	return bSuccess;
}