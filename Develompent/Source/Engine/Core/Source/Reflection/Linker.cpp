#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Linker.h"
#include "Reflection/LinkerManager.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/PackageFileCache.h"
#include "Reflection/Class.h"
#include "System/BaseFileSystem.h"

/*
==================
PackageFileSummary::PackageFileSummary
==================
*/
PackageFileSummary::PackageFileSummary()
{
	Sys_Memzero( this, sizeof( *this ) );
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
		InArchive << InValue.exportCount;
		InArchive << InValue.exportOffset;
		InArchive << InValue.importCount;
		InArchive << InValue.importOffset;
		InArchive << InValue.guid;
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


/*
==================
CLinkerLoad::CLinkerLoad
==================
*/
CLinkerLoad::CLinkerLoad( CObjectPackage* InRoot, const tchar* InFilename )
	: CLinker( InRoot, InFilename )
	, CArchive( InFilename )
	, bHasFinishedInitialization( false )
	, bHasSerializedPackageFileSummary( false )
	, bHasFoundExistingExports( false )
	, loader( nullptr )
{
	// Mark our loader as binary file
	SetType( AT_BinaryFile );
}

/*
==================
CLinkerLoad::GetPackageLinker
==================
*/
CLinkerLoad* CLinkerLoad::GetPackageLinker( CObjectPackage* InOuter, const tchar* InFilename )
{
	// See if there is already a linker for this package
	CLinkerLoad*	result = InOuter ? InOuter->GetLinker() : nullptr;
	if ( result )
	{
		return result;
	}

	// Try to load the linker
	std::wstring	newFilename;
	if ( !InFilename )
	{
		// Resolve filename from package name
		if ( !InOuter )
		{
			Warnf( TEXT( "Can't resolve package name\n" ) );
			return nullptr;
		}

		if ( !CPackageFileCache::Get().FindPackageFile( InOuter->GetName().c_str(), newFilename ) )
		{
			Warnf( TEXT( "Package file for '%s' not found\n" ), InOuter->GetFullName().c_str() );
			return nullptr;
		}
	}
	else
	{
		// Check that the file exists
		if ( !CPackageFileCache::Get().FindPackageFile( InFilename, newFilename ) )
		{
			Warnf( TEXT( "Package file '%s' not found\n" ), InFilename );
			return nullptr;
		}

		// Resolve package name from filename
		const tchar*	tmpFilename = InFilename;
		while ( true )
		{
			if ( Sys_Strstr( tmpFilename, PATH_SEPARATOR ) )
			{
				tmpFilename = Sys_Strstr( tmpFilename, PATH_SEPARATOR ) + Sys_Strlen( PATH_SEPARATOR );
			}
			else if ( Sys_Strstr( tmpFilename, TEXT( "/" ) ) )
			{
				tmpFilename = Sys_Strstr( tmpFilename, TEXT( "/" ) ) + 1;
			}
			else if ( Sys_Strstr( tmpFilename, SUBOBJECT_DELIMITER ) )
			{
				tmpFilename = Sys_Strstr( tmpFilename, SUBOBJECT_DELIMITER ) + 1;
			}
			else
			{
				break;
			}
		}

		if ( Sys_Strstr( tmpFilename, TEXT( "." ) ) )
		{
			*Sys_Strstr( tmpFilename, TEXT( "." ) ) = 0;
		}

		// Create a new package
		CObjectPackage*		package = CreatePackage( nullptr, tmpFilename );

		// If no package specified, use package from file
		if ( !InOuter )
		{
			if ( !package )
			{
				Warnf( TEXT( "Can't create a package '%s'\n" ), tmpFilename );
				return false;
			}

			InOuter = package;
			result	= InOuter->GetLinker();
		}
		// If we have InOuter and a new package not equal, it's mean what we loading a new file into an existing package, so reset the loader
		else if ( InOuter != package )
		{
			Logf( TEXT( "Loading a new file for existing package (%s, %s)\n" ), InOuter->GetFullName().c_str(), package->GetFullName().c_str() );
			CLinkerManager::Get().ResetLoaders( InOuter );
		}
	}

	// Create a new linker
	if ( !result )
	{
		// We will already have found the filename above
		Assert( InOuter );
		Assert( GetObjectSerializeContext().HasStartedLoading() );
		Assert( !newFilename.empty() );
		result = CreateLinker( InOuter, newFilename.c_str() );
		if ( result )
		{
			InOuter->SetLinker( result );
		}
	}

	// Success
	return result;
}

/*
==================
CLinkerLoad::CreateLinker
==================
*/
CLinkerLoad* CLinkerLoad::CreateLinker( CObjectPackage* InParent, const tchar* InFilename )
{
	// See whether there already is a linker for this parent/linker root
	CLinkerLoad*		linker = InParent ? InParent->GetLinker() : nullptr;
	if ( linker )
	{
		Logf( TEXT( "CLinkerLoad::CreateLinker: Found existing linker for '%s'\n" ), InParent->GetName().c_str() );
	}

	// Create a new linker if there isn't an existing one
	if ( !linker )
	{
		// If the linker failed on initialize we free allocated memory and return NULL
		linker = new CLinkerLoad( InParent, InFilename );	
		if ( !linker->Init() )
		{
			delete linker;
			return nullptr;
		}
	}

	return linker;
}

/*
==================
CLinkerLoad::Init
==================
*/
bool CLinkerLoad::Init()
{
	if ( !bHasFinishedInitialization )
	{
		// Create loader (CArchive) used for serialization
		if ( !CreateLoader() )
		{
			return false;
		}

		// Serialize the package file summary and presize the various arrays (import and export map)
		if ( !SerializePackageFileSummary() )
		{
			return false;
		}

		// Serialize the import map
		if ( !SerializeImportMap() )
		{
			return false;
		}

		// Serialize the export map
		if ( !SerializeExportMap() )
		{
			return false;
		}

		// Find existing objects matching exports and associate them with this linker
		if ( !FindExistingExports() )
		{
			return false;
		}

		// Finalize creation process
		if ( !FinalizeCreation() )
		{
			return false;
		}
	}

	return true;
}

/*
==================
CLinkerLoad::CreateLoader
==================
*/
bool CLinkerLoad::CreateLoader()
{
	if ( !loader )
	{
		// Create file reader used for serialization
		loader = g_FileSystem->CreateFileReader( arPath );
		if ( !loader )
		{
			Warnf( TEXT( "Failed to open file '%s'\n" ), arPath.c_str() );
			return false;
		}

		// Mark archive as binary file
		loader->SetType( AT_BinaryFile );
	}
	Assert( loader );

	// Error if linker is already loaded
	if( linkerRoot->GetLinker() )
	{
		Warnf( TEXT( "Linker for '%s' already exists\n" ), linkerRoot->GetName().c_str() );
		return false;
	}

	return true;
}

/*
==================
CLinkerLoad::SerializePackageFileSummary
==================
*/
bool CLinkerLoad::SerializePackageFileSummary()
{
	if ( !bHasSerializedPackageFileSummary )
	{
		// Read summary from file
		*this << summary;

		// Loader needs to be the same version
		loader->SetVer( summary.fileVersion );
		SetVer( summary.fileVersion );

		// Check tag
		if ( summary.tag != PACKAGE_FILE_TAG )
		{
			Warnf( TEXT( "Package '%s' has invalid tag. Engine Tag: 0x%X  Package Tag: 0x%X\n" ), arPath.c_str(), summary.tag, PACKAGE_FILE_TAG );
			return false;
		}

		// Validate the summary
		if ( summary.fileVersion < VER_MinVersion )
		{
			Warnf( TEXT( "Package '%s' was saved with an older version which is not backwards compatible with the current process. Min Required Version: 0x%X  Package Version: 0x%X" ) );
			return false;
		}

		// Warning user that package isn't saveable
		if ( summary.engineVersion > ENGINE_VERSION )
		{
			Warnf( TEXT( "Package '%s' isn't savable because it has been saved in the newly engine. Engine Version: 0x%X  Package's Engine Version: 0x%X\n" ), arPath.c_str(), ENGINE_VERSION, summary.engineVersion );
		}

		// Presize memory for import and export maps
		importMap.resize( summary.importCount );
		exportMap.resize( summary.exportCount );

		// Avoid serializing it again.
		bHasSerializedPackageFileSummary = true;
	}

	return true;
}

/*
==================
CLinkerLoad::SerializeImportMap
==================
*/
bool CLinkerLoad::SerializeImportMap()
{
	// Seek to the start of import map in a file
	if ( summary.importCount > 0 )
	{
		Seek( summary.importOffset );
	}

	// Serialize the import map
	for ( uint32 importObjIndex = 0; importObjIndex < summary.importCount; ++importObjIndex )
	{
		ObjectImport&	importObject = importMap[importObjIndex];
		*this << importObject;
	}

	// We done
	return true;
}

/*
==================
CLinkerLoad::SerializeExportMap
==================
*/
bool CLinkerLoad::SerializeExportMap()
{
	// Seek to the start of object map in a file
	if ( summary.exportCount > 0 )
	{
		Seek( summary.exportOffset );
	}

	// Serialize the object map
	for ( uint32 exportObjIndex = 0; exportObjIndex < summary.exportCount; ++exportObjIndex )
	{
		ObjectExport&	exportObject = exportMap[exportObjIndex];
		*this << exportObject;
	}

	// We done
	return true;
}

/*
==================
CLinkerLoad::FindExistingExports
==================
*/
bool CLinkerLoad::FindExistingExports()
{
	if ( !bHasFoundExistingExports )
	{
		// Only look for existing exports in the editor after it has started up
#if WITH_EDITOR
		if ( g_IsEditor && g_IsRunning )
		{
			for ( uint32 exportObjIndex = 0, exportObjsCount = exportMap.size(); exportObjIndex < exportObjsCount; ++exportObjIndex )
			{
				FindExistingExport( exportObjIndex );
			}
		}
#endif // WITH_EDITOR

		// Avoid duplicate work
		bHasFoundExistingExports = true;
	}
	return true;
}

/*
==================
CLinkerLoad::FindExistingExport
==================
*/
CObject* CLinkerLoad::FindExistingExport( uint32 InExportIndex )
{
	Assert( InExportIndex >= 0 && InExportIndex < exportMap.size() );
	ObjectExport&		exportObject = exportMap[InExportIndex];

	// If we were already found, leave early
	if ( exportObject.object )
	{
		return exportObject.object;
	}

	// Find the outer package for this object, if it's already loaded
	CObject*	outerObject = nullptr;
	if ( exportObject.outerIndex.IsNull() )
	{
		// This export's outer is the CPackage root of this loader
		outerObject = linkerRoot;
	}
	else if ( exportObject.outerIndex.IsExport() )
	{
		// If we have a package index, then we are in a group or other object, and we should look for it
		outerObject = FindExistingExport( exportObject.outerIndex.ToExport() );
	}
	else
	{
		// Our outer is actually an import
		outerObject = FindExistingImport( exportObject.outerIndex.ToImport() );
	}

	// If we found one, keep going. If we didn't find one, then this package has never been loaded before
	if ( outerObject )
	{
		// Find the class of this object
		CClass*		theClass = nullptr;
		if ( exportObject.classIndex.IsNull() )
		{
			theClass = CClass::StaticClass();
		}
		else
		{
			// Check if this object export is a non-native class, non-native classes are always exports.
			// If so, then use the outer object as a package
			CObject*	classPackage	= exportObject.classIndex.IsExport() ? linkerRoot : nullptr;
			const bool	bAnyPackage		= classPackage == nullptr;
			theClass	= ( CClass* )FindObjectFast( CClass::StaticClass(), classPackage, ImportExport( exportObject.classIndex ).objectName, false, bAnyPackage );
		}

		// If the class exists, try to find the object
		if ( theClass )
		{
			exportObject.object = FindObjectFast( theClass, outerObject, exportObject.objectName, true, false );

			// If we found an object, set it's linker to us
			if ( exportObject.object )
			{
				exportObject.object->SetLinker( this, InExportIndex );
			}
		}
	}

	return exportObject.object;
}

/*
==================
CLinkerLoad::FindExistingImport
==================
*/
CObject* CLinkerLoad::FindExistingImport( uint32 InImportIndex )
{
	Assert( InImportIndex >= 0 && InImportIndex < importMap.size() );
	ObjectImport&	importObject = importMap[InImportIndex];

	// The import object is already resolved just return it
	if ( importObject.object )
	{
		return importObject.object;
	}

	// Find the outer package for this object, if it's already loaded
	CObject*	outerObject = nullptr;
	if ( importObject.outerIndex.IsNull() )
	{
		// If the import outer is null then we have a package, resolve it
		return FindObjectFast( CObjectPackage::StaticClass(), nullptr, importObject.objectName, true, false );
	}
	// If out outer is an import, recurse to find it
	else if ( importObject.outerIndex.IsImport() )
	{
		outerObject = FindExistingImport( importObject.outerIndex.ToImport() );
	}
	// Otherwise out outer is actually an export from this package
	else
	{
		outerObject = FindExistingExport( importObject.outerIndex.ToExport() );
	}

	if ( outerObject )
	{
		// Find the class of this object
		CClass*		theClass = nullptr;
		if ( importObject.className == NAME_CClass || !importObject.className.IsValid() )
		{
			theClass = CClass::StaticClass();
		}
		else
		{
			theClass = ( CClass* )FindObjectFast( CClass::StaticClass(), nullptr, importObject.className, false, true );
		}

		// If the class exists, try to find the object
		if ( theClass )
		{
			return FindObjectFast( CClass::StaticClass(), outerObject, importObject.objectName, true, false );
		}
	}

	return nullptr;
}

/*
==================
CLinkerLoad::FinalizeCreation
==================
*/
bool CLinkerLoad::FinalizeCreation()
{
	if ( !bHasFinishedInitialization )
	{
		// Add this linker to the object manager's linker array
		CLinkerManager::Get().AddLoader( this );

		// Tell the root package guid
		if ( linkerRoot )
		{
			linkerRoot->SetGuid( summary.guid );
		}

		// Avoid duplicate work in the case of async linker creation
		bHasFinishedInitialization = true;
	}

	return true;
}

/*
==================
CLinkerLoad::CreateExport
==================
*/
CObject* CLinkerLoad::CreateExport( uint32 InExportIndex )
{
	// Map the object into our table
	ObjectExport&		exportObject = exportMap[InExportIndex];

	// Check whether we already loaded the object
	if ( !exportObject.object )
	{
		Assert( exportObject.objectName != NAME_None );
		Assert( GetObjectSerializeContext().HasStartedLoading() );

		// Get the object's class
		CClass*		loadClass = ( CClass* )IndexToObject( exportObject.classIndex );
		if ( !loadClass && !exportObject.classIndex.IsNull() )
		{
			Warnf( TEXT( "Unable to load %s because its class doesn't exist\n" ), exportObject.objectName.ToString().c_str() );
			return nullptr;
		}

		// If the class wasn't found and ClassIndex is null then its CClass
		if ( !loadClass )
		{
			loadClass = CClass::StaticClass();
		}

		Assert( loadClass );
		Assert( loadClass->GetClass() == CClass::StaticClass() );

		// Only CClass objects and CProperty objects of intrinsic classes can have OBJECT_Native set. Those property objects are never
		// serialized so we only have to worry about classes. If we encounter an object that is not a class and has OBJECT_Native set
		// we warn about it and remove the flag
		if ( ( exportObject.objectFlags & OBJECT_Native ) != 0 && !loadClass->IsChildOf<CField>() )
		{
			Warnf( TEXT( "%s %s has OBJECT_Native set but isn't a CField derived class\n" ), loadClass->GetName().c_str(), exportObject.objectName.ToString().c_str() );

			// Remove OBJECT_Native flag
			exportObject.objectFlags = exportObject.objectFlags & ~OBJECT_Native;
		}

		// We need preload LoadClass if it isn't intrinsic class
		if ( !loadClass->HasAnyClassFlags( CLASS_Intrinsic ) )
		{
			Preload( loadClass );
		}

		// Find or create the object's outer
		CObject*	thisParent = nullptr;
		if ( !exportObject.outerIndex.IsNull() )
		{
			thisParent = IndexToObject( exportObject.outerIndex );
		}
		else
		{
			thisParent = linkerRoot;
		}

		// If loading the object's Outer caused the object to be loaded or if it was a forced export package created
		// above, return it
		if ( exportObject.object )
		{
			return exportObject.object;
		}

		// If an outer that doesn't exist then log a warning
		if ( !thisParent )
		{
			const std::wstring		outerName = exportObject.outerIndex.IsNull() ? linkerRoot->GetFullName() : GetFullImportExportName( exportObject.outerIndex );
			Warnf( TEXT( "CreateExport: Failed to load Outer for resource '%s': %s\n" ), exportObject.objectName.ToString().c_str(), outerName.c_str() );
			return nullptr;
		}

		// Try to find existing object first in case we're a forced export to be able to reconcile
		CObject*	actualObjectWithTheName	= FindObjectFast( nullptr, thisParent, exportObject.objectName, true );

		// Find object after making sure it isn't already set. This would be bad as the code below NULLs it in a certain
		// case, which if it had been set would cause a linker detach mismatch
		Assert( !exportObject.object );
		if ( actualObjectWithTheName && actualObjectWithTheName->GetClass() == loadClass )
		{
			exportObject.object = actualObjectWithTheName;
		}

		// Object is found in memory
		if ( exportObject.object )
		{
			// Associate linker with object to avoid detachment mismatches
			exportObject.object->SetLinker( this, InExportIndex );

			// If this object was allocated but never loaded (components created by a constructor) make sure it gets loaded
			// Don't do this for any packages that have previously fully loaded as they may have in memory changes
			GetObjectSerializeContext().AddLoadedObject( exportObject.object );
			if ( !exportObject.object->HasAnyObjectFlags( OBJECT_WasLoaded ) && !linkerRoot->IsFullyLoaded() )
			{
				exportObject.object->AddObjectFlag( OBJECT_NeedLoad | OBJECT_NeedPostLoad | OBJECT_NeedPostLoadSubobjects | OBJECT_WasLoaded );
			}

			return exportObject.object;
		}

		// If we found an object with the name but it isn't child of LoadClass then this is error
		if ( actualObjectWithTheName && !actualObjectWithTheName->GetClass()->IsChildOf( loadClass ) )
		{
			Errorf( TEXT( "Failed import: class '%s' name '%s' outer '%s'. There is another object (of '%s' class) at the path" ),
					loadClass->GetName().c_str(), exportObject.objectName.ToString().c_str(), thisParent->GetName().c_str(), actualObjectWithTheName->GetClass()->GetName().c_str() );
			return nullptr;
		}

		// Create the export object, marking it with the appropriate flags to
		// indicate that the object's data still needs to be loaded
		ObjectFlags_t	objectLoadFlags = ( exportObject.objectFlags & OBJECT_Mask_Load ) | OBJECT_NeedLoad | OBJECT_NeedPostLoad | OBJECT_NeedPostLoadSubobjects | OBJECT_WasLoaded;

		// Otherwise we create a new object
		exportObject.object = CObject::StaticConstructObject( loadClass, thisParent, exportObject.objectName, objectLoadFlags );

		// Associate linker with object to avoid detachment mismatches
		if ( exportObject.object )
		{
			Logf( TEXT( "Created %s\n" ), exportObject.object->GetFullName().c_str() );
			exportObject.object->SetLinker( this, InExportIndex );

			// We created the object, but the data stored on disk for this object has not yet been loaded,
			// so add the object to the list of objects that need to be loadaded, which will be processed
			// in EndLoad()
			GetObjectSerializeContext().AddLoadedObject( exportObject.object );
		}
		else
		{
			Warnf( TEXT( "CreatedExport failed to construct object %s %s\n" ), loadClass->GetName().c_str(), exportObject.objectName.ToString().c_str() );
		}
	}

	return exportObject.object;
}

/*
==================
CLinkerLoad::CreateImport
==================
*/
CObject* CLinkerLoad::CreateImport( uint32 InImportIndex )
{
	// Map the object into our table
	ObjectImport&		importObject = importMap[InImportIndex];

	// Print warning if we found unreachable object when creating import
	if ( importObject.object && importObject.object->IsUnreachable() )
	{
		Warnf( TEXT( "Unreachable object found when creating import %s from linker %s\n" ), importObject.object->GetFullName().c_str(), GetPath().c_str() );
		importObject.object = nullptr;
	}

	// Check whether we already loaded the object
	if ( !importObject.object )
	{
		// Try to find existing version in memory first
		if ( !g_IsEditor && !g_IsCommandlet )
		{
			if ( CObjectPackage* classPackage = FindObjectFast<CObjectPackage>( nullptr, importObject.classPackage, false, false ) )
			{
				if ( CClass* findClass = FindObjectFast<CClass>( classPackage, importObject.className, false, false ) )
				{
					// Make sure the class has been loaded
					Preload( findClass );

					// Import is a toplevel package
					CObject*	findObject = nullptr;
					// TODO yehor.pohuliaka: Implement find object
				}
			}
		}

		// TODO yehor.pohuliaka: Implement find object
	}

	// If failed to resolve import print error message
	if ( !importObject.object )
	{
		const std::wstring		outerName = importObject.outerIndex.IsNull() ? linkerRoot->GetFullName() : GetFullImportExportName( importObject.outerIndex );
		Errorf( TEXT( "Failed to resolve import '%d' named '%s' in '%s'\n" ), InImportIndex, importObject.objectName.ToString().c_str(), outerName.c_str() );
	}

	return importObject.object;
}

/*
==================
CLinkerLoad::Preload
==================
*/
void CLinkerLoad::Preload( CObject* InObject )
{}

/*
==================
CLinkerLoad::IndexToObject
==================
*/
CObject* CLinkerLoad::IndexToObject( CPackageIndex& InIndex )
{
	if ( InIndex.IsExport() )
	{
		uint32		exportIndex = InIndex.ToExport();
		if ( exportIndex < 0 || exportIndex >= exportMap.size() )
		{
			Sys_Errorf( TEXT( "Invalid export object index=%d while reading %s. File is most likely corrupted" ), exportIndex, GetPath().c_str() );
			return nullptr;
		}

		return CreateExport( exportIndex );
	}
	else if ( InIndex.IsImport() )
	{
		uint32		importIndex = InIndex.ToImport();
		if ( importIndex < 0 || importIndex >= exportMap.size() )
		{
			Sys_Errorf( TEXT( "Invalid export object index=%d while reading %s. File is most likely corrupted" ), importIndex, GetPath().c_str() );
			return nullptr;
		}

		return CreateImport( importIndex );
	}
	else
	{
		return nullptr;
	}
}

/*
==================
CLinkerLoad::LoadAllObjects
==================
*/
void CLinkerLoad::LoadAllObjects( bool InIsForcePreload /* = false */ )
{
	// Load all export objects
	for ( uint32 exportObjId = 0, exportObjsCount = exportMap.size(); exportObjId < exportObjsCount; ++exportObjId )
	{
		CObject*	object = CreateExport( exportObjId );
		if ( object )
		{
			//AssertNoEntry();
		}
	}
	AssertNoEntry();
}

/*
==================
CLinkerLoad::GetExportClassName
==================
*/
CName CLinkerLoad::GetExportClassName( uint32 InExportIndex ) const
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
CLinkerLoad::Detach
==================
*/
void CLinkerLoad::Detach()
{
	// Detach all objects linked with this linker
	for ( uint32 exportObjIndex = 0, exportObjsCount = exportMap.size(); exportObjIndex < exportObjsCount; ++exportObjIndex )
	{
		if ( exportMap[exportObjIndex].object )
		{
			DetachExport( exportObjIndex );
		}
	}

	// Remove from object manager, if it has been added
	CLinkerManager::Get().RemoveLoaderFromObjectLoaders( this );

	// Empty out no longer used array
	importMap.clear();
	exportMap.clear();

	// Make sure we're never associated with LinkerRoot again
	if ( linkerRoot )
	{
		linkerRoot->SetLinker( nullptr );
		linkerRoot = nullptr;
	}

	// Destroy loader
	if ( loader )
	{
		delete loader;
		loader = nullptr;
	}
}

/*
==================
CLinkerSave::DetachExport
==================
*/
void CLinkerLoad::DetachExport( uint32 InExportIndex )
{
	Assert( InExportIndex >= 0 && InExportIndex < exportMap.size() );
	ObjectExport&		exportObject = exportMap[InExportIndex];
	Assert( exportObject.object );

	if ( !exportObject.object->IsValid() )
	{
		Sys_Errorf( TEXT( "Linker object %s %s.%s is invalid" ), GetExportClassName( InExportIndex ).ToString().c_str(), linkerRoot->GetName().c_str(), exportObject.objectName.ToString() );
	}

	const CLinkerLoad*	actualLinker = exportObject.object->GetLinker();
	if ( actualLinker != this )
	{
		CObject*	object = exportObject.object;
		Logf( TEXT( "Object				: %s\n" ), object->GetFullName().c_str() );
		Logf( TEXT( "Linker LinkerRoot	: %s\n" ), object->GetLinker() ? object->GetLinker()->linkerRoot->GetFullName().c_str() : TEXT( "None" ) );
		Logf( TEXT( "Detach LinkerRoot	: %s\n" ), linkerRoot->GetFullName().c_str() );
		Logf( TEXT( "Linker object %s %s.%s mislinked!\n" ), GetExportClassName( InExportIndex ).ToString().c_str(), linkerRoot->GetName().c_str(), exportObject.objectName.ToString().c_str() );
	}

	if ( exportObject.object->GetLinkerIndex() == INDEX_NONE )
	{
		Warnf( TEXT( "Linker object %s %s.%s was already detached\n" ), GetExportClassName( InExportIndex ).ToString().c_str(), linkerRoot->GetName().c_str(), exportObject.objectName.ToString().c_str() );
	}
	else
	{
		AssertMsg( exportObject.object->GetLinkerIndex() == InExportIndex, TEXT( "Mismatched linker index in CLinkerLoad::DetachExport for %s in %s. Linker index was supposed to be %d, was %d" ),
				   GetExportClassName( InExportIndex ).ToString().c_str(),
				   linkerRoot->GetName().c_str(), 
				   InExportIndex,
				   exportObject.object->GetLinkerIndex() );
	}

	exportObject.object->SetLinker( nullptr, INDEX_NONE );
}

/*
==================
CLinkerSave::IsLoading
==================
*/
bool CLinkerLoad::IsLoading() const
{
	return true;
}

/*
==================
CLinkerSave::Tell
==================
*/
uint32 CLinkerLoad::Tell()
{
	return loader->Tell();
}

/*
==================
CLinkerSave::Seek
==================
*/
void CLinkerLoad::Seek( uint32 InPosition )
{
	loader->Seek( InPosition );
}

/*
==================
CLinkerSave::Flush
==================
*/
void CLinkerLoad::Flush()
{
	loader->Flush();
}

/*
==================
CLinkerSave::IsEndOfFile
==================
*/
bool CLinkerLoad::IsEndOfFile()
{
	return loader->IsEndOfFile();
}

/*
==================
CLinkerSave::GetSize
==================
*/
uint32 CLinkerLoad::GetSize()
{
	return loader->GetSize();
}

/*
==================
CLinkerSave::Serialize
==================
*/
void CLinkerLoad::Serialize( void* InBuffer, uint32 InSize )
{
	loader->Serialize( InBuffer, InSize );
}

/*
==================
CLinkerSave::operator<<
==================
*/
CArchive& CLinkerLoad::operator<<( class CObject*& InValue )
{
	return *this;
}


/*
==================
CLinkerSave::CLinkerSave
==================
*/
CLinkerSave::CLinkerSave( CObjectPackage* InRoot, const tchar* InFilename )
	: CLinker( InRoot, InFilename )
	, CArchive( InFilename )
	, saver( nullptr )
{
	// Create file saver and mark archive as binary file
	saver = g_FileSystem->CreateFileWriter( InFilename, AW_NoFail );
	saver->SetType( AT_BinaryFile );
	SetType( AT_BinaryFile );

	// Set main summary info
	summary.tag				= PACKAGE_FILE_TAG;
	summary.engineVersion	= ENGINE_VERSION;
	summary.fileVersion		= VER_PACKAGE_LATEST;

	// Allocate indeces
	objectIndices.resize( CObjectGC::Get().GetReservedSizeAllocatedObjects() );
}

/*
==================
CLinkerSave::~CLinkerSave
==================
*/
CLinkerSave::~CLinkerSave()
{
	Detach();
}

/*
==================
CLinkerSave::Detach
==================
*/
void CLinkerSave::Detach()
{
	if ( saver )
	{
		delete saver;
		saver = nullptr;
	}
}

/*
==================
CLinkerSave::IsSaving
==================
*/
bool CLinkerSave::IsSaving() const
{
	return true;
}

/*
==================
CLinkerSave::Tell
==================
*/
uint32 CLinkerSave::Tell()
{
	return saver->Tell();
}

/*
==================
CLinkerSave::Seek
==================
*/
void CLinkerSave::Seek( uint32 InPosition )
{
	return saver->Seek( InPosition );
}

/*
==================
CLinkerSave::Flush
==================
*/
void CLinkerSave::Flush()
{
	saver->Flush();
}

/*
==================
CLinkerSave::IsEndOfFile
==================
*/
bool CLinkerSave::IsEndOfFile()
{
	return saver->IsEndOfFile();
}

/*
==================
CLinkerSave::GetSize
==================
*/
uint32 CLinkerSave::GetSize()
{
	return saver->GetSize();
}

/*
==================
CLinkerSave::Serialize
==================
*/
void CLinkerSave::Serialize( void* InBuffer, uint32 InSize )
{
	saver->Serialize( InBuffer, InSize );
}

/*
==================
CLinkerSave::operator<<
==================
*/
CArchive& CLinkerSave::operator<<( class CObject*& InValue )
{
	if ( !InValue )
	{
		return *this << CPackageIndex();
	}
	return *this << objectIndices[InValue->GetIndex()];
}