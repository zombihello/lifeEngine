/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LINKERLOAD_H
#define LINKERLOAD_H

#include "Reflection/Linker.h"

/**
 * @ingroup Core
 * @brief Archive that can reading compressed package
 */
class CCompressedPackageReader : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * @param InPath				Path to archive
	 * @param InCompressedChunks	Pointer to array containing information about (un)compressed chunks
	 * @param InCompressionFlags	Flags determining compression format associated with mapping
	 */
	CCompressedPackageReader( const std::wstring& InPath, std::vector<CompressedChunk>* InCompressedChunks, ECompressionFlags InCompressionFlags );

	/**
	 * @brief Destructor
	 */
	virtual ~CCompressedPackageReader();

	/**
	 * @brief Serialize data
	 * @param InBuffer	Pointer to buffer for serialize
	 * @param InSize	Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Precache the region that to be read soon
	 * This function will not change the current archive position
	 *
	 * @param InPrecacheOffset		Offset at which to begin precaching
	 * @param InPrecacheSize		Number of bytes to precache
	 */
	virtual void Precache( uint32 InPrecacheOffset, uint32 InPrecacheSize ) override;

	/**
	 * @brief Flushes cache and frees internal data
	 */
	virtual void FlushCache() override;

	/**
	 * @brief Set mapping from offsets/size that are going to be used for seeking and serialization to what
	 * is actually stored on disk
	 *
	 * @param InCompressedChunks	Pointer to array containing information about (un)compressed chunks
	 * @param InCompressionFlags	Flags determining compression format associated with mapping
	 * @return Return TRUE if archive supports translating offsets and uncompressing on read, otherwise FALSE
	 */
	virtual bool SetCompressionMap( std::vector<CompressedChunk>* InCompressedChunks, ECompressionFlags InCompressionFlags ) override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() override;

	/**
	 * @brief Set current position in archive
	 * @param InPosition	New position in archive
	 */
	virtual void Seek( uint32 InPosition ) override;

	/**
	 * @breif Is loading archive
	 * @return Return TRUE if archive loading, FALSE if archive saving
	 */
	virtual bool IsLoading() const override;

	/**
	 * Is end of file
	 * @return Return TRUE if end of file, otherwise FALSE
	 */
	virtual bool IsEndOfFile() override;

	/**
	 * @brief Get size of archive
	 * @return Return size of archive
	 */
	virtual uint32 GetSize() override;

private:
	/**
	 * @brief Precache chunk
	 */
	struct PrecacheChunk
	{
		/**
		 * @brie Constructor
		 */
		PrecacheChunk()
			: startPos( 0 )
			, endPos( 0 )
			, buffer( nullptr )
		{}

		uint32		startPos;	/**< Start position of current precache request */
		uint32		endPos;		/**< End position of current precache request */
		byte*		buffer;		/**< Buffer containing precached data */
	};

	/**
	 * @brief Whether the current precache buffer contains the passed in request
	 * 
	 * @param InRequestOffset	Offset in bytes from start of file
	 * @param InRequestSize		Size in bytes requested
	 * @return Return TRUE if buffer contains request, othwerise FALSE 
	 */
	FORCEINLINE bool PrecacheBufferContainsRequest( uint32 InRequestOffset, uint32 InRequestSize )
	{
		return InRequestOffset >= precacheChunk.startPos && ( InRequestOffset + InRequestSize <= precacheChunk.endPos );
	}

	/**
	 * @brief Find the compressed chunk index associated with the passed in offset
	 * 
	 * @param InRequestOffset	Offset in file to find associated chunk index for
	 * @return Return index into CompressedChunks array matching this offset
	 */
	uint32 FindCompressedChunkIndex( uint32 InRequestOffset ) const;

	/**
	 * @brief Precache compressed chunk
	 * @param InChunkIndex		Index of compressed chunk
	 */
	void PrecacheCompressedChunk( uint32 InChunkIndex );

	uint32								fileSize;				/**< Cached file size */
	uint32								uncompressedFileSize;	/**< Cached uncompressed file size */
	uint32								currentPos;				/**< Current position of archive */
	CArchive*							fileReader;				/**< File reader */
	std::vector<CompressedChunk>*		compressedChunks;		/**< Mapping of compressed/uncompresses sizes and offsets */
	ECompressionFlags					compressionFlags;		/**< Compression flags determining compression of compressedChunks */ 
	PrecacheChunk						precacheChunk;			/**< Precache chunk */
};

/**
 * @ingroup Core
 * @brief Handles loading LifeEngine package files, including reading CObject data from disk
 */
class CLinkerLoad : public CLinker, public CArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InRoot		The top-level CObjectPackage object for the package associated with this linker
	 * @param InFilename	The name of the file for this package
	 * @param InLoadFlags	Load flags determining behavior (see ELoadFlags)
	 */
	CLinkerLoad( CObjectPackage* InRoot, const tchar* InFilename, uint32 InLoadFlags );

	/**
	 * @brief Destructor
	 */
	virtual ~CLinkerLoad();

	/**
	 * @brief Find or create the linker for a package
	 *
	 * @param InOuter			Package if known, can be null
	 * @param InFilename		Package resource to load, can be empty if InOuter is valid
	 * @param InLoadFlags		Load flags determining behavior (see ELoadFlags)
	 * @return Return pointer to the loaded linker or NULL if the file didn't exist
	 */
	static CLinkerLoad* GetPackageLinker( CObjectPackage* InOuter, const tchar* InFilename, uint32 InLoadFlags );

	/**
	 * @brief Create linker
	 *
	 * @param InParent		Parent object to load into, can be NULL (most likely case)
	 * @param InFilename	Name of file on disk to load
	 * @param InLoadFlags	Load flags determining behavior (see ELoadFlags)
	 * @return Return a new CLinkerLoad object for InParent/InFilename
	 */
	static CLinkerLoad* CreateLinker( CObjectPackage* InParent, const tchar* InFilename, uint32 InLoadFlags );

	/**
	 * @brief Create the instance of an object as found in this linker
	 * 
	 * @param InClass		The class of the object
	 * @param InName		The name of the object
	 * @param InOuter		The outer that this object must be in (for finding objects in a specific group when there are multiple groups with the same name)
	 * @param Return the created object. If failed returns NULL
	 */
	CObject* CreateObject( CClass* InClass, const CName& InName, CObject* InOuter );

	/**
	 * @brief Load all objects in package
	 * @param InIsForcePreload		Whether to explicitly call Preload (serialize) right away instead of being called from EndLoadPackage()
	 */
	void LoadAllObjects( bool InIsForcePreload = false );

	/**
	 * @brief Detaches file loader and removes itself from array of loaders
	 */
	void Detach();

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool IsLoading() const override;

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() override;

	/**
	 * @brief Set current position in archive
	 *
	 * @param InPosition	New position in archive
	 */
	virtual void Seek( uint32 InPosition ) override;

	/**
	 * @brief Flush data
	 */
	virtual void Flush() override;

	/**
	 * Is end of file
	 * @return Return true if end of file, else return false
	 */
	virtual bool IsEndOfFile() override;

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32 GetSize() override;

	/**
	 * @brief Get linker
	 * @return Return pointer to linker if this archive is a CLinkerLoad or CLinkerSave, otherwise NULL
	 */
	virtual CLinker* GetLinker() const override;

	/**
	 * @brief Serialize data
	 *
	 * @param InBuffer	Pointer to buffer for serialize
	 * @param InSize	Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Precache the region that to be read soon
	 * This function will not change the current archive position
	 *
	 * @param InPrecacheOffset		Offset at which to begin precaching
	 * @param InPrecacheSize		Number of bytes to precache
	 */
	virtual void Precache( uint32 InPrecacheOffset, uint32 InPrecacheSize ) override;

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CName& InValue ) override;

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( const class CName& InValue ) override;

	/**
	 * @brief Serialize the object data for the specified object from the LifeEngine package file
	 * @note When this function exits, Object is guaranteed to contain the data stored that was stored on disk
	 * 
	 * Serialize the object data for the specified object from the LifeEngine package file. Loads any
	 * additional resources required for the object to be in a valid state to receive the loaded
	 * data, such as the object's Outer, Class, etc
	 *
	 * @param InObject	The object to load data for
	 */
	virtual void Preload( CObject* InObject ) override;

	/**
	 * @brief Get the class package name for the specified index in the export map
	 *
	 * @param InExportIndex		Export object index
	 * @return Return the class package name for the specified index in the export map
	 */
	CName GetExportClassPackage( uint32 InExportIndex ) const;

	/**
	 * @brief Is has this linker a loader
	 * @return Return TRUE if this linker has a loader, otherwise returns FALSE
	 */
	FORCEINLINE bool HasLoader() const
	{
		return loader != nullptr;
	}

	/**
	 * @brief Set a new loader archive (used by CFunction to serialize bytecode)
	 * @param InNewLoader		A new loader
	 */
	FORCEINLINE void SetLoader( CArchive* InNewLoader )
	{
		loader = InNewLoader;
	}

	/**
	 * @brief Get loader archive
	 * @return Return loader archive if it opened, otherwise NULL
	 */
	FORCEINLINE CArchive* GetLoader() const
	{
		return loader;
	}

protected:
	/**
	 * @brief Initialize linker
	 * @return Return TRUE if linker was successful initialized, otherwise returns FALSE
	 */
	bool Init();

	/**
	 * @brief Create loader used to serialize content
	 * @return Return TRUE if loader was successful created, otherwise returns FALSE
	 */
	bool CreateLoader();

	/**
	 * @brief Serializes the package file summary
	 * @return Return TRUE if package file summary was successful serialized, otherwise returns FALSE
	 */
	bool SerializePackageFileSummary();

	/**
	 * @brief Serializes the name map
	 * @return Return TRUE if name map was successful serialized, otherwise returns FALSE
	 */
	bool SerializeNameMap();

	/**
	 * @brief Serializes the import map
	 * @return Return TRUE if import map was successful serialized, otherwise returns FALSE
	 */
	bool SerializeImportMap();

	/**
	 * @brief Serializes the export map
	 * @return Return TRUE if export map was successful serialized, otherwise returns FALSE
	 */
	bool SerializeExportMap();

	/**
	 * @brief Creates the export hash
	 * @return Return TRUE if export hash was successful created, otherwise returns FALSE
	 */
	bool CreateExportHash();

	/**
	 * @brief Find existing exports in memory and match them up with this linker
	 * @return Return TRUE if this function was successful done, otherwise returns FALSE
	 */
	bool FindExistingExports();

	/**
	 * @brief Finalize linker creation
	 * @return Return TRUE if finalize creation was successful, otherwise returns FALSE
	 */
	bool FinalizeCreation();

	/**
	 * @brief Find existing export
	 *
	 * Before loading a persistent object from disk, this function can be used to discover
	 * the object in memory
	 *
	 * @param InExportIndex		The index of the export to hunt down
	 * @return Return the object that was found, or NULL if it wasn't found
	 */
	CObject* FindExistingExport( uint32 InExportIndex );

	/**
	 * @brief Find existing import
	 *
	 * @param InImportIndex		The index of the import to hunt down
	 * @return Return the object that was found, or NULL if it wasn't found
	 */
	CObject* FindExistingImport( uint32 InImportIndex );

private:
	static constexpr uint32		exportHashCount = 256;				/**< Export hash count */

	/**
	 * @brief Verify results
	 */
	enum EVerifyResult
	{
		VERIFY_Failed,		/**< Error occurred when verifying import */
		VERIFY_Success,		/**< Verify completed successfully */
		VERIFY_Redirected	/**< Verify completed successfully and followed a redirector */
	};

	/**
	 * @brief Detach an export object from the linker
	 * @param InExportIndex		The index of the export to detach
	 */
	void DetachExport( uint32 InExportIndex );

	/**
	 * @brief Create export object
	 *
	 * @param InExportIndex		Export object index
	 * @return Return created export object
	 */
	CObject* CreateExport( uint32 InExportIndex );

	/**
	 * @brief Create import object
	 *
	 * @param InImportIndex		Import object index
	 * @return Return created import object
	 */
	CObject* CreateImport( uint32 InImportIndex );

	/**
	 * @brief Find export index of the object
	 * 
	 * @param InClassName			The name of the class for the CObject
	 * @param InClassPackage		The name of the package that contains the class of the CObject
	 * @param InObjectName			The name of the CObject
	 * @param InExportOuterIndex	Export index of the outer of the CObject
	 * @return Return export index of the Object. If not found returns INDEX_NONE
	 */
	uint32 FindExportIndex( const CName& InClassName, const CName& InClassPackage, const CName& InObjectName, uint32 InExportOuterIndex );

	/**
	 * @brief Convert package index to object
	 *
	 * This function create an export or import object if it isn't.
	 * If the one already was created return it
	 *
	 * @param InIndex	Package index
	 * @return Return a pointer to object, if InIndex is null returns NULL
	 */
	CObject* IndexToObject( CPackageIndex& InIndex );

	/**
	 * @brief Safely verify that an import in the ImportMap points to a good object
	 * @param InImportIndex		The index into this packages ImportMap to verify
	 */
	void VerifyImportInner( uint32 InImportIndex );

	/**
	 * @brief A wrapper around VerifyImportInner
	 * A wrapper around VerifyImportInner. If the VerifyImportInner fails, this function
	 * will look for a CObjectRedirector that will point to the real location of the object
	 * 
	 * @param InImportIndex		The index into this packages ImportMap to verify
	 * @return Return verify import result
	 */
	EVerifyResult VerifyImport( uint32 InImportIndex );

	/**
	 * @brief Calculate hash for an export hash
	 * 
	 * @param InObject	Object name
	 * @return Return calculated hash for an export hash
	 */
	static FORCEINLINE uint32 GetHashBucket( CName InObject )
	{
		return InObject.GetHash() & ( exportHashCount - 1 );
	}

	bool						bHasFinishedInitialization;			/**< Whether we are already fully initialized */
	bool						bHasSerializedPackageFileSummary;	/**< Whether we already serialized the package file summary */
	bool						bHasFoundExistingExports;			/**< Whether we already matched up existing exports */
	uint32						exportHashIndex;					/**< Current index into export hash map, used by linker creation for spreading out hashing exports */
	uint32						exportHash[exportHashCount];		/**< Export hash */
	uint32						loadFlags;							/**< Flags determining loading behavior */
	CArchive*					loader;								/**< The archive that actually reads the raw data from disk */
};

#endif // !LINKERLOAD_H