#include "Reflection/Linker.h"
#include "Reflection/ObjectPackage.h"

/*
==================
PackageFileSummary::PackageFileSummary
==================
*/
PackageFileSummary::PackageFileSummary()
{
	Memory::Memzero( this, sizeof( *this ) );
}

/*
==================
PackageFileSummary::SetPackageFlags
==================
*/
void PackageFileSummary::SetPackageFlags( uint32 InPackageFlags )
{
	// Package summary flags should be set to FALSE for all transient and memory-only flags
	packageFlags = InPackageFlags & ~( PKG_MASK_TransientFlags );
}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, PackageFileSummary& InValue )
{
	InArchive << InValue.tag;

	// Only keep loading/saving if we match the magic
	if ( InValue.tag == PACKAGE_FILE_TAG )
	{
		InArchive << InValue.engineVersion;
		InArchive << InValue.fileVersion;
		InArchive << InValue.packageFlags;
		if ( InValue.fileVersion >= VER_NewSerializeName )
		{
			InArchive << InValue.nameCount;
			InArchive << InValue.nameOffset;
		}
		InArchive << InValue.exportCount;
		InArchive << InValue.exportOffset;
		InArchive << InValue.importCount;
		InArchive << InValue.importOffset;
	}
	else
	{
		Warnf( TEXT( "%s: Failed to get correct summary tag! 0x%X not 0x%X\n" ), InArchive.GetPath().c_str(), InValue.tag, PACKAGE_FILE_TAG );
		AssertMsg( false, TEXT( "%s: Bad summary tag" ), InArchive.GetPath().c_str() );
	}

	return InArchive;
}


/*
==================
CLinker::CLinker
==================
*/
CLinker::CLinker( CObjectPackage* InRoot, const tchar* InFilename )
	: linkerRoot( InRoot )
	, filename( InFilename )
{
	Assert( linkerRoot );
	Assert( InFilename );
}

/*
==================
CLinker::GetExportClassName
==================
*/
CName CLinker::GetExportClassName( uint32 InExportIndex ) const
{
	if ( InExportIndex >= 0 && InExportIndex < exportMap.size() )
	{
		const ObjectExport&		exportObject = exportMap[InExportIndex];
		if ( !exportObject.classIndex.IsNull() )
		{
			return ImportExport( exportObject.classIndex ).objectName;
		}
	}
	return NAME_CClass;
}

/*
==================
CLinker::GetImportPathName
==================
*/
std::wstring CLinker::GetImportPathName( uint32 InImportIndex ) const
{
	std::wstring	result;
	for ( CPackageIndex linkerIndex = CPackageIndex::FromImport( InImportIndex ); !linkerIndex.IsNull(); linkerIndex = ImportExport( linkerIndex ).outerIndex )
	{
		const ObjectResource&	objectResource = ImportExport( linkerIndex );
		bool					bSubobjectDelimiter = 
			GetClassName( linkerIndex ) != NAME_CObjectPackage && 
			( objectResource.outerIndex.IsNull() || GetClassName( objectResource.outerIndex ) == NAME_CObjectPackage );

		// Don't append a dot in the first iteration
		if ( !result.empty() )
		{
			if ( bSubobjectDelimiter )
			{
				result = std::wstring( SUBOBJECT_DELIMITER ) + result;
			}
			else
			{
				result = std::wstring( TEXT( "." ) ) + result;
			}
		}

		result = objectResource.objectName.ToString() + result;
	}

	return result;
}

/*
==================
CLinker::GetExportPathName
==================
*/
std::wstring CLinker::GetExportPathName( uint32 InExportIndex, const tchar* InFakeRoot /* = nullptr */ ) const
{
	std::wstring	result;
	bool			bHasOuterImport = false;

	for ( CPackageIndex linkerIndex = CPackageIndex::FromExport( InExportIndex ); !linkerIndex.IsNull(); linkerIndex = ImportExport( linkerIndex ).outerIndex )
	{
		bHasOuterImport |= linkerIndex.IsImport();
		const ObjectResource&	objectResource = ImportExport( linkerIndex );
		bool					bSubobjectDelimiter =
			( objectResource.outerIndex.IsNull() || GetClassName( objectResource.outerIndex ) == NAME_CObjectPackage )
			&& GetClassName( linkerIndex ) != NAME_CObjectPackage;

		// Don't append a dot in the first iteration
		if ( !result.empty() )
		{
			// If this export is not a CObjectPackage but this export's Outer is a CObjectPackage, we need to use subobject notation
			if ( bSubobjectDelimiter )
			{
				result = std::wstring( SUBOBJECT_DELIMITER ) + result;
			}
			else
			{
				result = std::wstring( TEXT( "." ) ) + result;
			}
		}

		result = objectResource.objectName.ToString() + result;
	}

	// If the export we are building the path of has an import in its outer chain, no need to append the LinkerRoot path
	if ( bHasOuterImport )
	{
		// Result already contains the correct path name for this export
		return result;
	}

	return ( InFakeRoot ? std::wstring( InFakeRoot ) : linkerRoot->GetPathName() ) + TEXT( "." ) + result;
}