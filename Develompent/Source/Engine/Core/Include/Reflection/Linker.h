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

#include "Reflection/ObjectResource.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief Information about a compressed chunk in a file
 */
struct CompressedChunk
{
	// I/O function
	friend CArchive& operator<<( CArchive& InArchive, CompressedChunk& InChunk );

	/**
	 * @brief Constructor
	 */
	CompressedChunk();

	uint32		uncompressedOffset;		/**< Original offset in uncompressed file */
	uint32		uncompressedSize;		/**< Uncompressed size in bytes */
	uint32		compressedOffset;		/**< Offset in compressed file */
	uint32		compressedSize;			/**< Compressed size in bytes */
};

/**
 * @ingroup Core
 * @brief A table of contents for a LifeEngine package file. Stored at the top of the file
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

	/**
	 * @brief Get package flags
	 * @return Return package flags
	 */
	uint32 GetPackageFlags() const
	{
		return packageFlags;
	}

	/**
	 * @brief Set the summary package flags while stripping out temporary flags (i.e. NewlyCreated, IsSaving)
	 * @param InPackageFlags	Package flags
	 */
	void SetPackageFlags( uint32 InPackageFlags );

private:
	uint32							packageFlags;		/**< The flags for the package */

public:
	uint32							tag;				/**< Magic tag compared against PACKAGE_FILE_TAG to ensure that package is a LifeEngine package */
	uint32							engineVersion;		/**< Engine version this package was saved with */
	uint32							fileVersion;		/**< The package file version number when this package was saved */
	uint32							totalHeaderSize;	/**< Total size of all information that needs to be read in to create a CLinkerLoad (This includes the package file summary, name, import and export tables) */
	uint32							compressionFlags;	/**< Flags used to compress the file on save and uncompress on load */
	std::vector<CompressedChunk>	compressedChunks;	/**< Array of compressed chunks in case this package was stored compressed */
	uint32							nameCount;			/**< Number of names used in this package */
	uint32							nameOffset;			/**< Location into the file on disk for the name data */
	uint32							exportCount;		/**< Number of exports contained in this package */
	uint32							exportOffset;		/**< Location into the file on disk for the ExportMap data */
	uint32							importCount;		/**< Number of imports contained in this package */
	uint32							importOffset;		/**< Location into the file on disk for the ImportMap data */
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

	/**
	 * @brief Get name map
	 * @return Return name map
	 */
	FORCEINLINE const std::vector<CName>& GetNames() const
	{
		return nameMap;
	}

	/**
	 * @brief Get name map
	 * @return Return name map
	 */
	FORCEINLINE std::vector<CName>& GetNames()
	{
		return nameMap;
	}

protected:
	std::vector<CName>			nameMap;		/**< Names used by objects contained within this package */
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
	 * @return Return the class name for the specified index in the export map. If either InExportIndex or its class index isn't valid returns NAME_CClass
	 */
	CName GetExportClassName( uint32 InExportIndex ) const;

	/**
	 * @brief Get the class name for the specified index in the export map
	 *
	 * @param InPackageIndex	Package index, must be export
	 * @return Return the class name for the specified index in the export map. If InPackageIndex isn't export returns NAME_None
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
	 * @return Return the full name of the CObject represented by the resource at InPackageIndex
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
	 * @brief Get the full name of the CObject represented by the specified package index
	 * 
	 * @param InPackageIndex			Package index for the resource to get the name for
	 * @param InFakeRoot				Optional name to replace use as the root package of this object instead of the linker
	 * @return Return the full name of the CObject represented by the resource at InPackageIndex. If InPackageIndex isn't export returns empty string
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

	/**
	 * @brief Get file name
	 * @return Return file name
	 */
	FORCEINLINE const std::wstring& GetFileName() const
	{
		return filename;
	}

protected:
	CObjectPackage*		linkerRoot;		/**< The top-level CObjectPackage object for the package associated with this linker */
	PackageFileSummary	summary;		/**< Table of contents for this package's file */
	std::wstring		filename;		/**< The name of the file for this package */
};

#endif // !LINKER_H