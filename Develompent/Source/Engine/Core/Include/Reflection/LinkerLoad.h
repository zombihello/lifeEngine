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
	 */
	CLinkerLoad( CObjectPackage* InRoot, const tchar* InFilename );

	/**
	 * @brief Find or create the linker for a package
	 *
	 * @param InOuter			Package if known, can be null
	 * @param InFilename		Package resource to load, must not be empty
	 * @return Return pointer to the loaded linker or NULL if the file didn't exist
	 */
	static CLinkerLoad* GetPackageLinker( CObjectPackage* InOuter, const tchar* InFilename );

	/**
	 * @brief Create linker
	 *
	 * @param InParent		Parent object to load into, can be NULL (most likely case)
	 * @param InFilename	Name of file on disk to load
	 * @return Return a new CLinkerLoad object for InParent/InFilename
	 */
	static CLinkerLoad* CreateLinker( CObjectPackage* InParent, const tchar* InFilename );

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
	 * @brief Serialize data
	 *
	 * @param InBuffer	Pointer to buffer for serialize
	 * @param InSize	Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) override;

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

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
	void Preload( CObject* InObject );

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
	 *
	 * @param InImportIndex		The index into this packages ImportMap to verify
	 */
	void VerifyImport( uint32 InImportIndex );

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
	CArchive*					loader;								/**< The archive that actually reads the raw data from disk */
};

#endif // !LINKERLOAD_H