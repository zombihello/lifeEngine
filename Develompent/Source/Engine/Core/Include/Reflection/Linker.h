/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LINKER_H
#define LINKER_H

#include <vector>
#include <unordered_map>

#include "Misc/Guid.h"
#include "Reflection/ObjectResource.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectSerializeContext.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief A table of contents for an LifeEngine package file. Stored at the top of the file
 */
struct PackageFileSummary
{
	/**
	 * @brief Constructor
	 */
	PackageFileSummary();

	/**
	 * @brief Serializes a package file summary from or into an archive
	 *
	 * @param InArchive		The archive to serialize from or to
	 * @param InValue		The value to serialize
	 */
	friend CArchive& operator<<( CArchive& InArchive, PackageFileSummary& InValue );

	uint32		tag;			/**< Magic tag compared against PACKAGE_FILE_TAG to ensure that package is an LifeEngine package */
	uint32		engineVersion;	/**< Engine version this package was saved with */
	uint32		fileVersion;	/**< The package file version number when this package was saved */
	uint32		exportCount;	/**< Number of exports contained in this package */
	uint32		exportOffset;	/**< Location into the file on disk for the ExportMap data */
	uint32		importCount;	/**< Number of imports contained in this package */
	uint32		importOffset;	/**< Location into the file on disk for the ImportMap data */
	CGuid		guid;			/**< Current id for this package */
};

/**
 * @ingroup Core
 * @brief Linker tables
 */
class CLinkerTables
{
public:
	/**
	 * @brief Get object import or export by index
	 * 
	 * @param InIndex	Package index to get
	 * @return Return the resource corresponding to this index
	 */
	FORCEINLINE ObjectResource& ImportExport( const CPackageIndex& InIndex )
	{
		Assert( !InIndex.IsNull() );
		if ( InIndex.IsImport() )
		{
			return Import( InIndex );
		}
		else
		{
			return Export( InIndex );
		}
	}

	/**
	 * @brief Get object import or export by index
	 *
	 * @param InIndex	Package index to get
	 * @return Return the resource corresponding to this index
	 */
	FORCEINLINE const ObjectResource& ImportExport( const CPackageIndex& InIndex ) const
	{
		Assert( !InIndex.IsNull() );
		if ( InIndex.IsImport() )
		{
			return Import( InIndex );
		}
		else
		{
			return Export( InIndex );
		}
	}

	/**
	 * @brief Get object import or export by index
	 * 
	 * @param InIndex	Package index to get
	 * @return Return the resource corresponding to this index, or NULL if the package index is null
	 */
	FORCEINLINE ObjectResource* ImportExportPtr( const CPackageIndex& InIndex )
	{
		if ( InIndex.IsImport() )
		{
			return ImportPtr( InIndex );
		}
		else
		{
			return ExportPtr( InIndex );
		}
	}

	/**
	 * @brief Get object import by index
	 * 
	 * @param InIndex	Package index to get
	 * @return Return the import corresponding to this index
	 */
	FORCEINLINE ObjectImport& Import( const CPackageIndex& InIndex )
	{
		Assert( InIndex.IsImport() && InIndex.ToImport() < importMap.size() && InIndex.ToImport() >= 0 );
		return importMap[InIndex.ToImport()];
	}

	/**
	 * @brief Get object import by index
	 *
	 * @param InIndex	Package index to get
	 * @return Return the import corresponding to this index
	 */
	FORCEINLINE const ObjectImport& Import( const CPackageIndex& InIndex ) const
	{
		Assert( InIndex.IsImport() && InIndex.ToImport() < importMap.size() && InIndex.ToImport() >= 0 );
		return importMap[InIndex.ToImport()];
	}

	/**
	 * @brief Get object import by index
	 * 
	 * @param InIndex	Package index to get
	 * @return Return the import corresponding to this index, or NULL if the package index is null or an export
	 */
	FORCEINLINE ObjectImport* ImportPtr( const CPackageIndex& InIndex )
	{
		if ( InIndex.IsImport() )
		{
			Assert( InIndex.ToImport() < importMap.size() && InIndex.ToImport() >= 0 );
			return &importMap[InIndex.ToImport()];
		}
		return nullptr;
	}

	 /**
	  * @brief Get object export by index
	  *
	  * @param InIndex	Package index to get
	  * @return Return the export corresponding to this index
	  */
	FORCEINLINE ObjectExport& Export( const CPackageIndex& InIndex )
	{
		Assert( InIndex.IsExport() && InIndex.ToExport() < exportMap.size() && InIndex.ToExport() >= 0 );
		return exportMap[InIndex.ToExport()];
	}

	/**
	 * @brief Get object export by index
	 *
	 * @param InIndex	Package index to get
	 * @return Return the export corresponding to this index
	 */
	FORCEINLINE const ObjectExport& Export( const CPackageIndex& InIndex ) const
	{
		Assert( InIndex.IsExport() && InIndex.ToExport() < exportMap.size() && InIndex.ToExport() >= 0 );
		return exportMap[InIndex.ToExport()];
	}

	 /**
	  * @brief Get object export by index
	  *
	  * @param InIndex	Package index to get
	  * @return Return the export corresponding to this index, or NULL if the package index is null or an import
	  */
	FORCEINLINE ObjectExport* ExportPtr( const CPackageIndex& InIndex )
	{
		if ( InIndex.IsExport() )
		{
			Assert( InIndex.ToExport() < exportMap.size() && InIndex.ToExport() >= 0 );
			return &exportMap[InIndex.ToExport()];
		}
		return nullptr;
	}

	/**
	 * @brief Get import map
	 * @return Return import map
	 */
	FORCEINLINE std::vector<ObjectImport>& GetImports()
	{
		return importMap;
	}

	/**
	 * @brief Get import map
	 * @return Return import map
	 */
	FORCEINLINE const std::vector<ObjectImport>& GetImports() const
	{
		return importMap;
	}

	/**
	 * @brief Get export map
	 * @return Return export map
	 */
	FORCEINLINE std::vector<ObjectExport>& GetExports()
	{
		return exportMap;
	}

	/**
	 * @brief Get export map
	 * @return Return export map
	 */
	FORCEINLINE const std::vector<ObjectExport>& GetExports() const
	{
		return exportMap;
	}

protected:
	std::vector<ObjectImport>	importMap;		/**< The list of ObjectImports found in the package */
	std::vector<ObjectExport>	exportMap;		/**< The list of ObjectExports found in the package */
};

/**
 * @ingroup Core
 * @brief Base linker
 * 
 * Manages the data associated with a LifeEngine package. Acts as the bridge between
 * the file on disk and the CObjectPackage object in memory for all LifeEngine package types
 */
class CLinker : public CLinkerTables
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InRoot		The top-level CObjectPackage object for the package associated with this linker
	 * @param InFilename	The name of the file for this package
	 */
	CLinker( CObjectPackage* InRoot, const tchar* InFilename );

	/**
	 * @brief Get the class name for the specified index in the export map
	 * 
	 * @param InExportIndex		Export object index
	 * @return Return the class name for the specified index in the export map. If either InExportIndex or its class index isn't valid returns NAME_None
	 */
	CName GetExportClassName( uint32 InExportIndex ) const;

	/**
	 * @brief Get the class name for the specified index in the export map
	 *
	 * @param InPackageIndex	Package index, must be export
	 * @return Return the class name for the specified index in the export map. If either InPackageIndex or its class index isn't valid returns NAME_None
	 */
	FORCEINLINE CName GetExportClassName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsExport() )
		{
			return GetExportClassName( InPackageIndex.ToExport() );
		}
		return NAME_None;
	}

	/**
	 * @brief Get the class name for the specified index in the import map
	 *
	 * @param InImportIndex		Import object index
	 * @return Return the class name for the specified index in the import map. If InImportIndex isn't valid returns NAME_None
	 */
	FORCEINLINE CName GetImportClassName( uint32 InImportIndex ) const
	{
		if ( InImportIndex >= 0 && InImportIndex < importMap.size() )
		{
			return importMap[InImportIndex].className;
		}
		return NAME_None;
	}

	/**
	 * @brief Get the class name for the specified index in the import map
	 *
	 * @param InPackageIndex		Package index, must be import
	 * @return Return the class name for the specified index in the import map. If InPackageIndex isn't valid returns NAME_None
	 */
	FORCEINLINE CName GetImportClassName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportClassName( InPackageIndex.ToImport() );
		}
		return NAME_None;
	}

	/**
	 * @brief Get the class name for the specified package index
	 * 
	 * @param InPackageIndex	Package index
	 * @return Return the class name for the specified package index. If InPackageIndex is null returns NAME_None
	 */
	FORCEINLINE CName GetClassName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportClassName( InPackageIndex );
		}
		else if ( InPackageIndex.IsExport() )
		{
			return GetExportClassName( InPackageIndex );
		}
		return NAME_None;
	}

	/**
	 * @brief Get the path name of the CObject represented by the specified import
	 * @note Can be used with FindObject
	 * 
	 * @param InImportIndex		The index into the import map for the resource to get the name for
	 * @return Return the path of the CObject represented by the resource at InImportIndex
	 */
	std::wstring GetImportPathName( uint32 InImportIndex ) const;

	/**
	 * @brief Get the path name of the CObject represented by the specified import
	 * @note Can be used with FindObject
	 * 
	 * @param InPackageIndex	Package index for the resource to get the name for
	 * @return Return the path name of the CObject represented by the resource at InPackageIndex, or the empty string if this isn't an import
	 */
	FORCEINLINE std::wstring GetImportPathName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportPathName( InPackageIndex.ToImport() );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the path name of the CObject represented by the specified export
	 * @note Can be used with FindObject
	 * 
	 * @param InExportIndex					Index into the export map for the resource to get the name for
	 * @param InFakeRoot					Optional name to replace use as the root package of this object instead of the linker
	 * @return Return the path name of the CObject represented by the resource at InExportIndex
	 */
	std::wstring GetExportPathName( uint32 InExportIndex, const tchar* InFakeRoot = nullptr ) const;
	
	/**
	 * @brief Get the path name of the CObject represented by the specified export
	 * @note Can be used with FindObject
	 * 
	 * @param InPackageIndex				Package index for the resource to get the name for
	 * @param InFakeRoot					Optional name to replace use as the root package of this object instead of the linker
	 * @return Return the path name of the CObject represented by the resource at InPackageIndex, or the empty string if this isn't an export
	 */
	FORCEINLINE std::wstring GetExportPathName( CPackageIndex InPackageIndex, const tchar* InFakeRoot = nullptr ) const
	{
		if ( InPackageIndex.IsExport() )
		{
			return GetExportPathName( InPackageIndex.ToExport(), InFakeRoot );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the path name of the CObject represented by the specified package index
	 * @note Can be used with FindObject
	 * 
	 * @param InPackageIndex	Package index
	 * @return Return the path name of the CObject represented by the resource at InPackageIndex, or the empty string if this is null
	 */
	FORCEINLINE std::wstring GetPathName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportPathName( InPackageIndex );
		}
		else if ( InPackageIndex.IsExport() )
		{
			return GetExportPathName( InPackageIndex );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the full name of the CObject represented by the specified import
	 * 
	 * @param InImportIndex		Index into the ImportMap for the resource to get the name for
	 * @return Return the full name of the CObject represented by the resource at InImportIndex. If InImportIndex isn't valid returns empty string
	 */
	FORCEINLINE std::wstring GetImportFullName( uint32 InImportIndex ) const
	{
		if ( InImportIndex >= 0 && InImportIndex < importMap.size() )
		{
			return importMap[InImportIndex].className.ToString() + TEXT( " " ) + GetImportPathName( InImportIndex );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the full name of the CObject represented by the specified package index
	 * 
	 * @param InPackageIndex	Package index for the resource to get the name for
	 * @return Return the full name of the UObject represented by the resource at InPackageIndex
	 */
	FORCEINLINE std::wstring GetImportFullName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportFullName( InPackageIndex.ToImport() );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the full name of the CObject represented by the specified export
	 * 
	 * @param InExportIndex				Index into the ExportMap for the resource to get the name for
	 * @param InFakeRoot				Optional name to replace use as the root package of this object instead of the linker
	 * @return Return the full name of the CObject represented by the resource at InExportIndex. If InExportIndex isn't valid returns empty string
	 */
	FORCEINLINE std::wstring GetExportFullName( uint32 InExportIndex, const tchar* InFakeRoot = nullptr ) const
	{
		if ( InExportIndex >= 0 && InExportIndex < exportMap.size() )
		{
			CPackageIndex	classIndex = exportMap[InExportIndex].classIndex;
			CName			className = classIndex.IsNull() ? NAME_CClass : ImportExport( classIndex ).objectName;
			return className.ToString() + TEXT( " " ) + GetExportPathName( InExportIndex, InFakeRoot );
		}
		return TEXT( " " );
	}

	/**
	 * @brief Get the full name of the UObject represented by the specified package index
	 * 
	 * @param InPackageIndex			Package index for the resource to get the name for
	 * @param InFakeRoot				Optional name to replace use as the root package of this object instead of the linker
	 * @return Return the full name of the CObject represented by the resource at InPackageIndex
	 */
	FORCEINLINE std::wstring GetExportFullName( CPackageIndex InPackageIndex, const tchar* InFakeRoot = nullptr ) const
	{
		if ( InPackageIndex.IsExport() )
		{
			return GetExportFullName( InPackageIndex.ToExport(), InFakeRoot );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the full name of the UObject represented by the specified export
	 * 
	 * @param InPackageIndex	Package index
	 * @return Return the path name of the UObject represented by the resource at PackageIndex, or the empty string if this is null
	 */
	FORCEINLINE std::wstring GetFullImportExportName( CPackageIndex InPackageIndex ) const
	{
		if ( InPackageIndex.IsImport() )
		{
			return GetImportFullName( InPackageIndex );
		}
		else if ( InPackageIndex.IsExport() )
		{
			return GetExportFullName( InPackageIndex );
		}
		return TEXT( "" );
	}

	/**
	 * @brief Get the top-level CObjectPackage object for the package associated with this linker
	 * @return Return the top-level CObjectPackage object for the package associated with this linker
	 */
	FORCEINLINE CObjectPackage* GetLinkerRoot() const
	{
		return linkerRoot;
	}

	/**
	 * @brief Get table of contents for this package's file
	 * @return Return table of contents for this package's file
	 */
	FORCEINLINE PackageFileSummary& GetSummary()
	{
		return summary;
	}

	/**
	 * @brief Get table of contents for this package's file
	 * @return Return table of contents for this package's file
	 */
	FORCEINLINE const PackageFileSummary& GetSummary() const
	{
		return summary;
	}

protected:
	CObjectPackage*		linkerRoot;		/**< The top-level CObjectPackage object for the package associated with this linker */
	PackageFileSummary	summary;		/**< Table of contents for this package's file */
	std::wstring		filename;		/**< The name of the file for this package */
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
	 * @brief Get the serialization context of objects
	 * @return Return the serialization context of objects
	 */
	static FORCEINLINE ObjectSerializeContext& GetObjectSerializeContext()
	{
		static ObjectSerializeContext	s_ObjectSerializeContext;
		return s_ObjectSerializeContext;
	}

	/**
	 * @brief Load all objects in package
	 * @param InIsForcePreload	Whether to explicitly call Preload (serialize) right away instead of being called from EndLoad()
	 */
	void LoadAllObjects( bool InIsForcePreload = false );

	/**
	 * @brief Detaches file loader and removes itself from array of loaders
	 */
	void Detach();

	/**
	 * @brief Whether linker has finished initialization
	 * @return Return TRUE if initialized and FALSE if pending
	 */
	FORCEINLINE bool HasFinishedInitializtion() const
	{
		return bHasFinishedInitialization;
	}

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
	 * @brief Get an export's class name
	 * 
	 * @param InExportIndex		The index of the export
	 * @return Return an export's class name if InExportIndex and the export's class index are valid, otherwise returns NAME_Class
	 */
	CName GetExportClassName( uint32 InExportIndex ) const;

	/**
	 * @brief Serialize the object data for the specified object from the LifeEngine package file
	 * 
	 * Serialize the object data for the specified object from the LifeEngine package file. Loads any
	 * additional resources required for the object to be in a valid state to receive the loaded
	 * data, such as the object's Outer, Class, etc
	 * 
	 * When this function exits, Object is guaranteed to contain the data stored that was stored on disk
	 * 
	 * @param InObject	The object to load data for
	 */
	void Preload( CObject* InObject );

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

	bool		bHasFinishedInitialization;			/**< Whether we are already fully initialized */
	bool		bHasSerializedPackageFileSummary;	/**< Whether we already serialized the package file summary */
	bool		bHasFoundExistingExports;			/**< Whether we already matched up existing exports */
	CArchive*	loader;								/**< The archive that actually reads the raw data from disk */
};

/**
 * @ingroup Core
 * @brief Handles saving LifeEngine package files
 */
class CLinkerSave : public CLinker, public CArchive
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InRoot		The top-level CObjectPackage object for the package associated with this linker
	 * @param InFilename	The name of the file for this package
	 */
	CLinkerSave( CObjectPackage* InRoot, const tchar* InFilename );

	/**
	 * @brief Destructor
	 */
	~CLinkerSave();

	/**
	 * @brief Detaches file saver and hence file handle.
	 */
	void Detach();

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override;

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
	 * @brief Get object indeces
	 * Location of the resource for a CObject is stored in the ObjectIndices array using the CObject's Index
	 * 
	 * @return Return object indeces
	 */
	FORCEINLINE std::vector<CPackageIndex>& GetObjectIndices()
	{
		return objectIndices;
	}

private:
	CArchive*						saver;			/**< The archive that actually writes the data to disk */
	std::vector<CPackageIndex>		objectIndices;	/**< Index array. Location of the resource for a CObject is stored in the ObjectIndices array using the CObject's Index */
};

#endif // !LINKER_H