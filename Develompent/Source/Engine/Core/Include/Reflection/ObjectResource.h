/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTRESOURCE_H
#define OBJECTRESOURCE_H

#include "Core.h"
#include "Misc/Guid.h"
#include "System/Archive.h"
#include "System/Name.h"
#include "Reflection/Object.h"

/**
 * @ingroup Core
 * @brief Package index
 * 
 * Wrapper for index into a CLnker's ImportMap or ExportMap.
 * Values greater than zero indicate that this is an index into the ExportMap. The
 * actual array index will be (CPackageIndex - 1).
 *
 * Values less than zero indicate that this is an index into the ImportMap. The actual
 * array index will be (-CPackageIndex - 1)
 */
class CPackageIndex
{
public:
	/**
	 * @brief Constructor, sets the value to null
	 */
	FORCEINLINE CPackageIndex()
		: index( 0 )
	{}

	/**
	 * @brief Is import index
	 * @return Return TRUE if this is an index into the import map
	 */
	FORCEINLINE bool IsImport() const
	{
		return index < 0;
	}

	/**
	 * @brief Is export index
	 * @return Return TRUE if this is an index into the export map
	 */
	FORCEINLINE bool IsExport() const
	{
		return index > 0;
	}

	/**
	 * @brief Is null index
	 * @return Return TRUE if this null (i.e. neither an import nor an export)
	 */
	FORCEINLINE bool IsNull() const
	{
		return index == 0;
	}

	/**
	 * @brief Convert to import index
	 * @return Return the index into the import map
	 */
	FORCEINLINE int32 ToImport() const
	{
		Assert( IsImport() );
		return -index - 1;
	}

	/**
	 * @brief Convert to export index
	 * @return Return the index into the export map
	 */
	FORCEINLINE int32 ToExport() const
	{
		Assert( IsExport() );
		return index - 1;
	}

	/**
	 * @brief Get the raw value
	 * @return Return the raw value, for debugging purposes
	 */
	FORCEINLINE int32 ForDebugging() const
	{
		return index;
	}

	/**
	 * @brief Create package index from an import index
	 * 
	 * @param InImportIndex		Import index
	 * @return Return created a CPackageIndex from an import index
	 */
	FORCEINLINE static CPackageIndex FromImport( int32 InImportIndex )
	{
		Assert( InImportIndex >= 0 );
		return CPackageIndex( -InImportIndex - 1 );
	}

	/**
	 * @brief Create package index from an export index
	 *
	 * @param InExportIndex		Export index
	 * @return Return created a CPackageIndex from an export index
	 */
	FORCEINLINE static CPackageIndex FromExport( int32 InExportIndex )
	{
		Assert( InExportIndex >= 0 );
		return CPackageIndex( InExportIndex + 1 );
	}

	/**
	 * @brief Compare package indecies for equality
	 * @return Return TRUE if package indecies is equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator==( const CPackageIndex& InOther ) const
	{
		return index == InOther.index;
	}

	/**
	 * @brief Compare package indecies for inequality
	 * @return Return TRUE if package indecies isn't equal, otherwise returns FALSE
	 */
	FORCEINLINE bool operator!=( const CPackageIndex& InOther ) const
	{
		return index != InOther.index;
	}

	/**
	 * @brief Compare package indecies for less
	 * @return Return TRUE if the index is less the InOther, otherwise returns FALSE
	 */
	FORCEINLINE bool operator<( const CPackageIndex& InOther ) const
	{
		return index < InOther.index;
	}

	/**
	 * @brief Compare package indecies for more
	 * @return Return TRUE if the index is more the InOther, otherwise returns FALSE
	 */
	FORCEINLINE bool operator>( const CPackageIndex& InOther ) const
	{
		return index > InOther.index;
	}

	/**
	 * @brief Compare package indecies for less or equal
	 * @return Return TRUE if the index is less or equal the InOther, otherwise returns FALSE
	 */
	FORCEINLINE bool operator<=( const CPackageIndex& InOther ) const
	{
		return index <= InOther.index;
	}

	/**
	 * @brief Compare package indecies for more or equal
	 * @return Return TRUE if the index is more or equal the InOther, otherwise returns FALSE
	 */
	FORCEINLINE bool operator>=( const CPackageIndex& InOther ) const
	{
		return index >= InOther.index;
	}

	/**
	 * @brief Serializes a package index value from or into an archive
	 *
	 * @param InArchive		The archive to serialize from or to
	 * @param InValue		The value to serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, CPackageIndex& InValue )
	{
		InArchive << InValue.index;
		return InArchive;
	}

private:
	/**
	 * @brief Internal constructor, sets the index directly
	 * @param InIndex	Index
	 */
	FORCEINLINE explicit CPackageIndex( int32 InIndex )
		: index( InIndex )
	{}

	int32		index;	/**< Values greater than zero indicate that this is an index into the ExportMap. The actual array index will be (CPackageIndex - 1).  Values less than zero indicate that this is an index into the ImportMap. The actual array index will be (-CPackageIndex - 1) */
};

/**
 * @ingroup Core
 * @brief Object resource
 * 
 * Base class for CObject resource types. CObjectResources are used to store CObjects on disk
 * via CLinker's ImportMap (for resources contained in other packages) and ExportMap (for resources
 * contained within the same package)
 */
struct ObjectResource
{
	CName			objectName; /**< The name of the CObject represented by this resource. Serialized */
	CPackageIndex	outerIndex;	/**< Location of the resource for this resource's Outer. Values of 0 indicate that this resource represents a top-level CPackage object (the linker's LinkerRoot). Serialized */

	/**
	 * @brief Constructor
	 */
	ObjectResource();

	/**
	 * @brief Constructor
	 * @param InObject	Object
	 */
	ObjectResource( CObject* InObject );
};

/**
 * @ingroup Core
 * @brief Object export
 * 
 * CObject resource type for objects that are contained within this package and can
 * be referenced by other packages
 */
struct ObjectExport : public ObjectResource
{
	/**
	 * @brief Constructor
	 */
	ObjectExport();

	/**
	 * @brief Constructor
	 * @param InObject	Object
	 */
	ObjectExport( CObject* InObject );

	/**
	 * @brief Serializes an object export from or into an archive
	 *
	 * @param InArchive		The archive to serialize from or to
	 * @param InValue		The value to serialize
	 */
	friend CArchive& operator<<( CArchive& InArchive, ObjectExport& InValue );

	CPackageIndex		classIndex;		/**< Location of the resource for this export's class (if non-zero). A value of zero indicates that this export represents a CClass object; there is no resource for this export's class object. Serialized */
	CPackageIndex		superIndex;		/**< Location of the resource for this export's super field (parent). Only valid if this export represents a CStruct object. A value of zero indicates that the object represented by this export isn't a CStruct-derived object. Serialized */
	uint64				serialSize;		/**< The number of bytes to serialize when saving/loading this export's CObject. Serialized */
	uint64				serialOffset;	/**< The location (into the CLinker's underlying file reader archive) of the beginning of the data for this export's CObject. Used for verification only. Serialized */
	CObject*			object;			/**< The CObject represented by this export. Assigned the first time CreateExport is called for this export. Transient */
	CGuid				packageGuid;	/**< The GUID for the original package file. Serialized */
};

/**
 * @ingroup Core
 * @brief Object import
 *
 * CObject resource type for objects that are referenced by this package, but contained
 * within another package
 */
struct ObjectImport : public ObjectResource
{
	/**
	 * @brief Constructor
	 */
	ObjectImport();

	/**
	 * @brief Constructor
	 * @param InObject	Object
	 */
	ObjectImport( CObject* InObject );

	/**
	 * @brief Serializes an object import from or into an archive
	 *
	 * @param InArchive		The archive to serialize from or to
	 * @param InValue		The value to serialize
	 */
	friend CArchive& operator<<( CArchive& InArchive, ObjectImport& InValue );

	CName					classPackage;		/**< The name of the package that contains the class of the CObject represented by this resource. Serialized */
	CName					className;			/**< The name of the class for the CObject represented by this resource. Serialized */
	int32					sourceIndex;		/**< Index into SourceLinker's ExportMap for the export associated with this import's CObject. Transient */
	CObject*				object;				/**< The CObject represented by this resource. Assigned the first time CreateImport is called for this import. Transient */
	class CLinkerLoad*		sourceLinker;		/**< The linker that contains the original ObjectExport resource associated with this import. Transient */
};

#endif // !OBJECTRESOURCE_H