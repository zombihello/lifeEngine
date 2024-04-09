#include "Logger/LoggerMacros.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/LinkerManager.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectRedirector.h"
#include "Reflection/Class.h"
#include "System/PackageFileCache.h"

/*
==================
CLinkerLoad::CLinkerLoad
==================
*/
CLinkerLoad::CLinkerLoad( CObjectPackage* InRoot, const tchar* InFilename, uint32 InLoadFlags )
	: CLinker( InRoot, InFilename )
	, CArchive( InFilename )
	, bHasFinishedInitialization( false )
	, bHasSerializedPackageFileSummary( false )
	, bHasFoundExistingExports( false )
	, exportHashIndex( 0 )
	, loadFlags( InLoadFlags )
	, loader( nullptr )
{
	// We check that ExportHashCount must be power of two
	static_assert( ( exportHashCount & ( exportHashCount - 1 ) ) == 0, "ExportHashCount must be power of two" );

	// Mark our loader as binary file
	SetType( AT_BinaryFile );
}

/*
==================
CLinkerLoad::~CLinkerLoad
==================
*/
CLinkerLoad::~CLinkerLoad()
{
	// Linkers can only be deleted by CLinkerManager
	if ( !CLinkerManager::Get().IsDeletingLinkers() )
	{
		Sys_Error( TEXT( "Linkers can only be deleted by CLinkerManager" ) );
	}

	// Detaches linker
	Detach();
	Assert( !loader );
}

/*
==================
CLinkerLoad::GetPackageLinker
==================
*/
CLinkerLoad* CLinkerLoad::GetPackageLinker( CObjectPackage* InOuter, const tchar* InFilename, uint32 InLoadFlags )
{
	// See if there is already a linker for this package
	CLinkerLoad*	result = InOuter ? InOuter->GetLinker() : nullptr;
	if ( result )
	{
		return result;
	}

	// Get package name from file name
	std::wstring	tmpBuffer = InFilename ? InFilename : TEXT( "" );
	const tchar*	packageName = tmpBuffer.data();
	if ( !tmpBuffer.empty() )
	{
		while ( true )
		{
			if ( L_Strstr( packageName, PATH_SEPARATOR ) )
			{
				packageName = L_Strstr( packageName, PATH_SEPARATOR ) + L_Strlen( PATH_SEPARATOR );
			}
			else if ( L_Strstr( packageName, TEXT( "/" ) ) )
			{
				packageName = L_Strstr( packageName, TEXT( "/" ) ) + 1;
			}
			else if ( L_Strstr( packageName, SUBOBJECT_DELIMITER ) )
			{
				packageName = L_Strstr( packageName, SUBOBJECT_DELIMITER ) + 1;
			}
			else
			{
				break;
			}
		}

		if ( L_Strstr( packageName, TEXT( "." ) ) )
		{
			*L_Strstr( packageName, TEXT( "." ) ) = 0;
		}
	}

	// If we no have outer and package name its error and we can't find for this package a linker
	if ( !InOuter && !packageName )
	{
		Warnf( TEXT( "Can't resolve package name\n" ) );
		return nullptr;
	}

	// Figure out our target package
	CObjectPackage*		tagetPackage = nullptr;
	if ( InOuter )
	{
		tagetPackage = InOuter;
	}
	else
	{
		// If we no have outer or we have a new package name then try find already created package in memory
		tagetPackage = FindObjectFast<CObjectPackage>( nullptr, packageName );
		if ( tagetPackage && tagetPackage->GetOuter() )
		{
			tagetPackage = nullptr;
		}
	}

	// Verify that the file exists
	std::wstring	newFilename;
	if ( !CPackageFileCache::Get().FindPackageFile( tagetPackage ? tagetPackage->GetName().c_str() : packageName, newFilename ) )
	{
		// This is a memory-only package and so it has no linker and this is ok
		if ( tagetPackage && tagetPackage->HasAnyPackageFlags( PKG_MASK_InMemoryOnly ) )
		{
			return nullptr;
		}
		else
		{
			Warnf( TEXT( "Package file '%s' not found\n" ), InFilename );
			return nullptr;
		}
	}

	// If we not found target package try create it
	if ( !tagetPackage )
	{
		tagetPackage = CObjectPackage::CreatePackage( nullptr, packageName );
		if ( !tagetPackage )
		{
			Warnf( TEXT( "Can't create a package '%s'\n" ), packageName );
			return nullptr;
		}
	}

	// See if the Linker is already loaded for the TargetPackage we've found
	if ( InOuter != tagetPackage )
	{
		result = tagetPackage->GetLinker();
	}

	// Create a new linker	
	if ( !result )
	{
		// We will already have found the filename above
		Assert( !newFilename.empty() );
		Assert( tagetPackage );
		Assert( CObjectPackage::GetObjectSerializeContext().HasStartedLoading() );
		result = CreateLinker( tagetPackage, newFilename.c_str(), InLoadFlags );
		if ( result )
		{
			// Remember linker and file path
			tagetPackage->SetLinker( result );
			tagetPackage->SetPackagePath( newFilename );
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
CLinkerLoad* CLinkerLoad::CreateLinker( CObjectPackage* InParent, const tchar* InFilename, uint32 InLoadFlags )
{
	// See whether there already is a linker for this parent/linker root
	CLinkerLoad*	linker = InParent ? InParent->GetLinker() : nullptr;
	if ( linker )
	{
		Logf( TEXT( "CLinkerLoad::CreateLinker: Found existing linker for '%s'\n" ), InParent->GetName().c_str() );
	}

	// Create a new linker if there isn't an existing one
	if ( !linker )
	{
		// If the linker failed on initialize we free allocated memory and return NULL
		linker = new CLinkerLoad( InParent, InFilename, InLoadFlags );
		if ( !linker->Init() )
		{
			// Detach linker and delete it
			linker->Detach();
			CLinkerManager::Get().DeleteLoader( linker );
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

		// Create hash table for exports
		if ( !CreateExportHash() )
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
	if ( linkerRoot->GetLinker() )
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

		// Loader needs to be the same filter editor only flag
		if ( summary.GetPackageFlags() & PKG_FilterEditorOnly )
		{
			loader->SetFilterEditorOnly( true );
			SetFilterEditorOnly( true );
		}

		// In cooked packages we serialize properties in binary form instead of safer but slower tagged form
		if ( summary.GetPackageFlags() & PKG_Cooked )
		{
			loader->SetWantBinaryPropertySerialization( true );
			SetWantBinaryPropertySerialization( true );
		}

		// Check tag
		if ( summary.tag != PACKAGE_FILE_TAG )
		{
			Warnf( TEXT( "Package '%s' has invalid tag. Engine Tag: 0x%X  Package Tag: 0x%X\n" ), arPath.c_str(), summary.tag, PACKAGE_FILE_TAG );
			return false;
		}

		// Validate the summary
		if ( summary.fileVersion < VER_MinVersion )
		{
			Warnf( TEXT( "Package '%s' was saved with an older version which is not backwards compatible with the current process. Min Required Version: 0x%X  Package Version: 0x%X\n" ), VER_MinVersion, summary.fileVersion );
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
		ObjectImport& importObject = importMap[importObjIndex];
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
		ObjectExport& exportObject = exportMap[exportObjIndex];
		*this << exportObject;
	}

	// We done
	return true;
}

/*
==================
CLinkerLoad::CreateExportHash
==================
*/
bool CLinkerLoad::CreateExportHash()
{
	// Initialize hash on first iteration
	if ( exportHashIndex == 0 )
	{
		for ( uint32 index = 0; index < exportHashCount; ++index )
		{
			exportHash[index] = INDEX_NONE;
		}
	}

	// Set up export hash
	while ( exportHashIndex < exportMap.size() )
	{
		ObjectExport&	exportObject = exportMap[exportHashIndex];
		const uint32	hash = GetHashBucket( exportObject.objectName );
		
		exportObject.hashNext = exportHash[hash];
		exportHash[hash] = exportHashIndex;
		++exportHashIndex;
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

		// Some platforms will run out of file handles. So, this will close the package
		CObjectPackage::GetObjectSerializeContext().AddDelayedLinkerClosePackage( this );

		// Tell the root package flags
		if ( linkerRoot )
		{
			linkerRoot->SetPackageFlags( summary.GetPackageFlags() );
		}

		// Avoid duplicate work in the case of async linker creation
		bHasFinishedInitialization = true;
	}

	return true;
}

/*
==================
CLinkerLoad::GetExportClassPackage
==================
*/
CName CLinkerLoad::GetExportClassPackage( uint32 InExportIndex ) const
{
	const ObjectExport&			exportObject = exportMap[InExportIndex];
	if ( exportObject.classIndex.IsImport() )
	{
		const ObjectImport&		importObject = Import( exportObject.classIndex );
		return ImportExport( importObject.outerIndex ).objectName;
	}
	else if ( !exportObject.classIndex.IsNull() )
	{
		// The export's class is contained within the same package
		return linkerRoot->GetCName();
	}
	else
	{
		return NAME_Core;
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
		if ( object && ( InIsForcePreload || object->GetClass() == CClass::StaticClass() ) )
		{
			if ( object->HasAnyObjectFlags( OBJECT_NeedLoad ) )
			{
				Preload( object );
			}
		}
	}

	// Mark package as having been fully loaded
	if ( linkerRoot )
	{
		linkerRoot->MarkAsFullyLoaded();
	}
}

/*
==================
CLinkerLoad::CreateObject
==================
*/
CObject* CLinkerLoad::CreateObject( CClass* InClass, const CName& InName, CObject* InOuter )
{
	Assert( InOuter );
	CPackageIndex		outerIndex;
	
	// If the outer isn't the outermost of the package, then we get the linker index of the outer as the outer index that we look in
	if ( InOuter != InOuter->GetOutermost() )
	{
		// We need the linker index of the outer to look in, which means that the outer must have been actually 
		// loaded off disk, and not just CreatePackage
		uint32			linkerIndex = InOuter->GetLinkerIndex();
		Assert( linkerIndex != INDEX_NONE );
		outerIndex		= CPackageIndex::FromExport( InOuter->GetLinkerIndex() );
	}

	// Find and create the export object
	uint32	index = FindExportIndex( InClass->GetCName(), InClass->GetOuter()->GetCName(), InName, !outerIndex.IsNull() ? outerIndex.ToExport() : INDEX_NONE );
	if ( index != INDEX_NONE )
	{
		return CreateExport( index );
	}

	// Since we didn't find it, see if we can find an object redirector with the same name
	index = FindExportIndex( NAME_CObjectRedirector, NAME_Core, InName, !outerIndex.IsNull() ? outerIndex.ToExport() : INDEX_NONE );

	// If we found a redirector, create it, and move on down the line
	if ( index != INDEX_NONE )
	{
		// Create the redirector and load it
		CObjectRedirector*		objectRedirector = ( CObjectRedirector* )CreateExport( index );
		Preload( objectRedirector );

		// If we found what it was point to, then return it
		CObject*				destinationObject = objectRedirector->GetDestinationObject();
		if ( destinationObject && destinationObject->GetClass() == InClass )
		{
			// Broadcast that we followed a redirector successfully
			CObjectRedirector::onObjectRedirectorFollowed.Broadcast( filename, objectRedirector );

			// Return the object we are being redirected to
			return destinationObject;
		}
	}

	// Otherwise we not found the object :(
	return nullptr;
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
		Assert( exportObject.objectName != NAME_None || !( exportObject.objectFlags & OBJECT_Public ) );
		Assert( CObjectPackage::GetObjectSerializeContext().HasStartedLoading() );

		// Get the object's class
		CClass*		loadClass = ( CClass* )IndexToObject( exportObject.classIndex );
		if ( !loadClass && !exportObject.classIndex.IsNull() )
		{
			Warnf( TEXT( "CreateExport: Unable to load %s because its class doesn't exist\n" ), exportObject.objectName.ToString().c_str() );
			return nullptr;
		}

		// If the class wasn't found and ClassIndex is null then its CClass
		if ( !loadClass )
		{
			loadClass = CClass::StaticClass();
		}

		Assert( loadClass );
		Assert( loadClass->GetClass() == CClass::StaticClass() );

		// Only objects child of CField can have OBJECT_Native set. If we encounter an object that is not a class and has OBJECT_Native set
		// we warn about it and remove the flag
		if ( ( exportObject.objectFlags & OBJECT_Native ) != 0 && !loadClass->IsChildOf<CField>() )
		{
			Warnf( TEXT( "CreateExport: %s %s has OBJECT_Native set but isn't a CField derived class\n" ), loadClass->GetName().c_str(), exportObject.objectName.ToString().c_str() );

			// Remove OBJECT_Native flag
			exportObject.objectFlags = exportObject.objectFlags & ~OBJECT_Native;
		}

		// We need preload LoadClass
		Preload( loadClass );

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

		// If loading the object's Outer caused the object to be loaded, return it
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

		// Find existing object after making sure it isn't already set. This would be bad as the code below NULLs it in a certain
		// case, which if it had been set would cause a linker detach mismatch
		Assert( !exportObject.object );
		CObject*	actualObjectWithTheName	= FindObjectFast( nullptr, thisParent, exportObject.objectName, true );
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
			CObjectPackage::GetObjectSerializeContext().AddLoadedObject( exportObject.object );
			if ( !exportObject.object->HasAnyObjectFlags( OBJECT_WasLoaded ) && !linkerRoot->IsFullyLoaded() )
			{
				exportObject.object->AddObjectFlag( OBJECT_NeedLoad | OBJECT_NeedPostLoad | OBJECT_WasLoaded );
			}

			return exportObject.object;
		}

		// If we found an object with the name but it isn't child of LoadClass then this is error
		if ( actualObjectWithTheName && !actualObjectWithTheName->GetClass()->IsChildOf( loadClass ) )
		{
			Errorf( TEXT( "CreateExport: Failed to import object '%s' with class '%s' and outer '%s'. There is another object (of '%s' class) at the path\n" ),
					exportObject.objectName.ToString().c_str(), loadClass->GetName().c_str(), thisParent->GetName().c_str(), actualObjectWithTheName->GetClass()->GetName().c_str() );
			return nullptr;
		}

		// Create the export object, marking it with the appropriate flags to indicate that the object's data still needs to be loaded
		ObjectFlags_t	objectLoadFlags = ( exportObject.objectFlags & OBJECT_MASK_Load ) | OBJECT_NeedLoad | OBJECT_NeedPostLoad | OBJECT_WasLoaded;

		// Otherwise we create a new object
		exportObject.object = CObject::StaticConstructObject( loadClass, thisParent, exportObject.objectName, objectLoadFlags );
		if ( g_IsInitialLoad || CObjectGC::Get().IsOpenForDisregardForGC() )
		{
			exportObject.object->AddToRoot();
		}

		// Associate linker with object to avoid detachment mismatches
		if ( exportObject.object )
		{
			exportObject.object->SetLinker( this, InExportIndex );

			// We created the object, but the data stored on disk for this object has not yet been loaded,
			// so add the object to the list of objects that need to be loadaded, which will be processed
			// in EndLoadPackage()
			CObjectPackage::GetObjectSerializeContext().AddLoadedObject( exportObject.object );
			Logf( TEXT( "CreateExport: Created %s\n" ), exportObject.object->GetFullName().c_str() );
		}
		else
		{
			Warnf( TEXT( "CreateExport: Failed to construct object %s %s\n" ), loadClass->GetName().c_str(), exportObject.objectName.ToString().c_str() );
		}

		// If the created object is child of CStruct or CClass, we need to init it
		// TODO yehor.pohuliaka: Maybe it need move to Preload function
		if ( exportObject.object )
		{
			// If it's a struct or class, set its parent
			if ( IsA<CStruct>( exportObject.object ) )
			{
				if ( !exportObject.superIndex.IsNull() )
				{
					( ( CStruct* )exportObject.object )->SetSuperStruct( ( CStruct* )IndexToObject( exportObject.superIndex ) );
				}

				// If it's a class, bind it to C++
				if ( IsA<CClass>( exportObject.object ) )
				{
					( ( CClass* )exportObject.object )->Bind();
				}
			}
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
		Warnf( TEXT( "CreateImport: Unreachable object found when creating import %s from linker %s\n" ), importObject.object->GetFullName().c_str(), GetPath().c_str() );
		importObject.object = nullptr;
	}

	// Check whether we already loaded the object
	if ( !importObject.object )
	{
		// Try to load the object if we no have source linker
		EVerifyResult		verifyImportResult = VERIFY_Success;
		if ( !importObject.sourceLinker )
		{
			verifyImportResult = VerifyImport( InImportIndex );
		}

		// If we have source index then create export for this import
		if ( importObject.sourceIndex != INDEX_NONE )
		{
			Assert( importObject.sourceLinker );
			
			// VerifyImport may have already created the import and SourceIndex has changed to point to the actual redirected object
			if ( !importObject.object || verifyImportResult != VERIFY_Redirected )
			{
				importObject.object = importObject.sourceLinker->CreateExport( importObject.sourceIndex );
			}
			CObjectPackage::GetObjectSerializeContext().IncrementImportCount();
			CLinkerManager::Get().AddLoaderWithNewImports( this );
		}

		// If failed to resolve import print error message
		if ( !importObject.object )
		{
			const std::wstring		outerName = importObject.outerIndex.IsNull() ? linkerRoot->GetFullName() : GetFullImportExportName( importObject.outerIndex );
			Errorf( TEXT( "CreateImport: Failed to resolve import '%d' named '%s' in '%s'\n" ), InImportIndex, importObject.objectName.ToString().c_str(), outerName.c_str() );
		}
	}

	return importObject.object;
}

/*
==================
CLinkerLoad::CreateImport
==================
*/
uint32 CLinkerLoad::FindExportIndex( const CName& InClassName, const CName& InClassPackage, const CName& InObjectName, uint32 InExportOuterIndex )
{
	// Find the object's export index
	uint32		hash = GetHashBucket( InObjectName );
	for ( uint32 index = exportHash[hash]; index != INDEX_NONE; index = exportMap[index].hashNext )
	{
		// Make sure what index is valid
		if ( index < 0 || index >= exportMap.size() )
		{
			Warnf( TEXT( "FindExportIndex: Invalid index [%d/%d] while attempting to find the object's export index '%s' with LinkerRoot '%s'\n" ),
				   index, exportMap.size(), InObjectName.ToString().c_str(), linkerRoot->GetName().c_str() );
			break;
		}

		// If we have same class name, class package, object name and outer index then we found it!
		if ( exportMap[index].objectName == InObjectName && 
			 ( ( exportMap[index].outerIndex.IsNull() && InExportOuterIndex == INDEX_NONE ) || exportMap[index].outerIndex.ToExport() == InExportOuterIndex ) &&
			 GetExportClassPackage( index ) == InClassPackage &&
			 GetExportClassName( index ) == InClassName )
		{
			return index;
		}
	}

	// If an object with the exact class wasn't found, look for objects with a subclass of the requested class 
	for ( uint32 exportIndex = 0, numExports = exportMap.size(); exportIndex < numExports; ++exportIndex )
	{
		ObjectExport&		exportObject = exportMap[exportIndex];
		if ( exportObject.objectName == InObjectName && ( ( exportObject.outerIndex.IsNull() && InExportOuterIndex == INDEX_NONE ) || exportObject.outerIndex.ToExport() == InExportOuterIndex ) )
		{
			// See if this export's class inherits from the requested class
			CClass*		exportClass = Cast<CClass>( IndexToObject( exportObject.classIndex ) );
			for ( CClass* parentClass = exportClass; parentClass; parentClass = parentClass->GetSuperClass() )
			{
				if ( parentClass->GetCName() == InClassName )
				{
					return exportIndex;
				}
			}
		}
	}

	return INDEX_NONE;
}

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
			Sys_Error( TEXT( "Invalid export object index=%d while reading %s. File is most likely corrupted" ), exportIndex, GetPath().c_str() );
			return nullptr;
		}

		return CreateExport( exportIndex );
	}
	else if ( InIndex.IsImport() )
	{
		uint32		importIndex = InIndex.ToImport();
		if ( importIndex < 0 || importIndex >= importMap.size() )
		{
			Sys_Error( TEXT( "Invalid export object index=%d while reading %s. File is most likely corrupted" ), importIndex, GetPath().c_str() );
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
CLinkerLoad::VerifyImportInner
==================
*/
void CLinkerLoad::VerifyImportInner( uint32 InImportIndex )
{
	Assert( IsLoading() );
	Assert( CObjectPackage::GetObjectSerializeContext().HasStartedLoading() );

	// Map the object into our table
	ObjectImport&	importObject = importMap[InImportIndex];

	// Already verified, or not relevent in this context
	if ( ( importObject.sourceLinker && importObject.sourceIndex != INDEX_NONE ) ||
		 importObject.classPackage == NAME_None ||
		 importObject.className == NAME_None ||
		 importObject.objectName == NAME_None )
	{
		return;
	}

	// Build the import object name on the stack and only once to avoid string temporaries
	CObject*		package = nullptr;
	CObjectPackage* tmpPackage = nullptr;
	std::wstring	importObjectName;
	importObjectName.reserve( 256 );
	importObject.objectName.AppendString( importObjectName );

	// Find or load the linker load that contains the ObjectExport for this import
	if ( importObject.outerIndex.IsNull() && importObject.className != NAME_CObjectPackage )
	{
		Errorf( TEXT( "VerifyImportInner: %s has an inappropriate outermost, it was probably saved with a deprecated outer (file: %s)\n" ), importObjectName.c_str(), GetPath().c_str() );
		importObject.sourceLinker = nullptr;
		return;
	}
	// This import is a CObjectPackage, load it
	else if ( importObject.outerIndex.IsNull() )
	{
		// Our outer is a CObjectPackage
		Assert( importObject.className == NAME_CObjectPackage );
		tmpPackage = CObjectPackage::CreatePackage( nullptr, importObjectName.c_str() );
		importObject.sourceLinker = GetPackageLinker( tmpPackage, nullptr, loadFlags );

		// If we found source linker then this package is on HDD. Otherwise its may be either memory only package, or the one isn't on HDD
		if ( importObject.sourceLinker )
		{
			importObject.object = tmpPackage;
			CObjectPackage::GetObjectSerializeContext().IncrementImportCount();
			CLinkerManager::Get().AddLoaderWithNewImports( this );
			return;
		}
	}
	else
	{
		// This import outer is also an import, so recurse verify into it
		if ( importObject.outerIndex.IsImport() )
		{
			VerifyImport( importObject.outerIndex.ToImport() );

			// If the import outer object has been resolved but not linker has been found, we am import to a memory only package (i.e. compiled in)
			ObjectImport&	outerImport = Import( importObject.outerIndex );
			if ( !outerImport.sourceLinker && outerImport.object )
			{
				ObjectImport*	topObjectImport;
				for ( topObjectImport = &outerImport; topObjectImport->outerIndex.IsImport(); topObjectImport = &Import( topObjectImport->outerIndex ) )
				{
					// For loop does what we need
				}

				// Assign temp package to resolve the object in memory when there is no source linker available only if the package is PKG_MASK_InMemoryOnly
				CObjectPackage*		topPackage = Cast<CObjectPackage>( topObjectImport->object );
				if ( topPackage && topPackage->HasAnyPackageFlags( PKG_MASK_InMemoryOnly ) )
				{
					// This is an import to a memory-only package, just search for it in the package
					tmpPackage = topPackage;
				}
			}

			// Copy the SourceLinker from the ObjectImport for our Outer if the SourceLinker hasn't been set yet,
			// Otherwise we may be overwriting a re-directed linker and SourceIndex is already from the redirected one
			// or we had an assigned package and our linker is already set
			if ( !importObject.sourceLinker )
			{
				importObject.sourceLinker = outerImport.sourceLinker;
			}
		}
		else
		{
			Assert( importObject.outerIndex.IsExport() );
		}

		// Now that we have a linker for the import, resolve the the export map index of our import in that linker.
		// If we do not have a linker, then this import is native
		if ( importObject.sourceLinker )
		{
			// Assign the linker root of the source linker as the package we are looking for
			package = importObject.sourceLinker->linkerRoot;
		
			// Find this import within its existing linker
			uint32		hash = GetHashBucket( importObject.objectName );
			for ( uint32 index = importObject.sourceLinker->exportHash[hash]; index != INDEX_NONE; index = importObject.sourceLinker->exportMap[index].hashNext )
			{
				if ( index < 0 || index >= importObject.sourceLinker->exportMap.size() )
				{
					Warnf( TEXT( "VerifyImportInner: Invalid index [%d/%d] while attempting to import '%s' with LinkerRoot '%s'\n" ),
						   index, importObject.sourceLinker->exportMap.size(), importObjectName.c_str(), importObject.sourceLinker->linkerRoot->GetName().c_str() );
					break;
				}
				else
				{
					ObjectExport&		sourceExport = importObject.sourceLinker->exportMap[index];
					if ( sourceExport.objectName == importObject.objectName &&
						 // If we are not explicitly looking for a redirector, skip for now as it will be properly handled in CreateImport 
						 ( importObject.className == NAME_CObjectRedirector ) == ( importObject.sourceLinker->GetExportClassName( index ) == NAME_CObjectRedirector ) )
					{
						// At this point, SourceExport is an ObjectExport in another linker that looks like it
						// matches the ObjectImport we're trying to load - double check that we have the correct one
						if ( importObject.outerIndex.IsImport() )
						{
							ObjectImport&	outerImport = Import( importObject.outerIndex );

							// OuterImport is the ObjectImport for this resource's Outer
							if ( outerImport.sourceLinker )
							{
								// If the import for our Outer doesn't have a SourceIndex, it means that
								// we haven't found a matching export for our Outer yet. This should only
								// be the case if our Outer is a top-level CObjectPackage
								if ( outerImport.sourceIndex == INDEX_NONE )
								{
									// At this point, we know our Outer is a top-level CObjectPackage, so
									// if the ObjectExport that we found has an Outer that is
									// not a linker root, this isn't the correct resource
									if ( !sourceExport.outerIndex.IsNull() )
									{
										continue;
									}
								}
								// If our import and its outer share the same source linker, make sure the outer source index matches as expected, otherwise, skip resolving this import
								else if ( importObject.sourceLinker == outerImport.sourceLinker )
								{
									if ( CPackageIndex::FromExport( outerImport.sourceIndex ) != sourceExport.outerIndex )
									{
										continue;
									}
								}
								// If the import and its outer do not share a source linker, validate the import entry of the outer in the source linker matches otherwise skip resolveing the outer
								else
								{
									Assert( sourceExport.outerIndex.IsImport() );
									ObjectImport&	sourceExportOuter = importObject.sourceLinker->Import( sourceExport.outerIndex );
									if ( sourceExportOuter.objectName != outerImport.objectName )
									{
										continue;
									}
									else
									{
										if ( sourceExportOuter.className != outerImport.className || sourceExportOuter.classPackage != outerImport.classPackage )
										{
											Warnf( TEXT( "VerifyImportInner: Resolved outer import with a different class: import class '%s.%s', package class '%s.%s'. Resave %s to fix\n" ),
												   outerImport.classPackage.ToString().c_str(), outerImport.className.ToString().c_str(), sourceExportOuter.classPackage.ToString(), sourceExportOuter.className.ToString().c_str(), linkerRoot->GetName().c_str() );
										}
									}
								}
							}
						}

						if ( !( sourceExport.objectFlags & OBJECT_Public ) )
						{
							Warnf( TEXT( "VerifyImportInner: Can't import private object %s %s\n" ), importObject.className.ToString().c_str(), GetImportFullName( index ).c_str() );
							return;
						}

						// Found the ObjectExport for this import
						if ( ( importObject.className != importObject.sourceLinker->GetExportClassName( index ) ) || ( importObject.classPackage != importObject.sourceLinker->GetExportClassPackage( index ) ) )
						{
							Warnf( TEXT( "VerifyImportInner: Resolved import with a different class: import class '%s.%s', package class '%s.%s'. Resave %s to fix\n" ),
								   importObject.classPackage.ToString().c_str(), 
								   importObject.className.ToString().c_str(), 
								   importObject.sourceLinker->GetExportClassPackage( index ).ToString().c_str(), 
								   importObject.sourceLinker->GetExportClassName( index ).ToString().c_str(), 
								   linkerRoot->GetName().c_str() );
						}

						importObject.sourceIndex = index;
						break;
					}
				}
			}
		}
	}

	// Assign Package to resolve the object in memory when there is no source linker available only if the package is MemoryOnly
	bool	bCameFromMemoryOnlyPackage = false;
	if ( !package && tmpPackage && tmpPackage->HasAnyPackageFlags( PKG_MASK_InMemoryOnly ) )
	{
		// This is an import to a memory-only package, just search for it in the package
		package = tmpPackage;
		bCameFromMemoryOnlyPackage = true;

		// Except if we are looking for _the_ package.. in which case we are looking for TmpPakage, so we are done
		if ( importObject.classPackage == NAME_Core && importObject.className == NAME_CObjectPackage && !tmpPackage->GetOuter() && importObject.objectName == tmpPackage->GetCName() )
		{
			importObject.object = tmpPackage;
			CObjectPackage::GetObjectSerializeContext().IncrementImportCount();
			CLinkerManager::Get().AddLoaderWithNewImports( this );
			return;
		}
	}

	// If not found in file, see if it's a public native transient class or field
	if ( importObject.sourceIndex == INDEX_NONE && ( package || loadFlags & LOAD_FindIfFail ) )
	{
		CObject*		classPackage = FindObjectFast<CObjectPackage>( nullptr, importObject.classPackage );
		if ( classPackage )
		{
			CClass*		findClass = FindObjectFast<CClass>( classPackage, importObject.className );
			if ( findClass )
			{
				CObject*	findOuter = package;
				if ( importObject.outerIndex.IsImport() )
				{
					// If this import corresponds to an intrinsic class, OuterImport's Object will be NULL if this import
					// belongs to the same package that the import's class is in; in this case, the package is the correct Outer to use
					// for finding this object otherwise, this import represents a field of an intrinsic class, and OuterImport's Object 
					// should be non-NULL (the object that contains the field)
					ObjectImport&	outerImport = Import( importObject.outerIndex );
					if ( outerImport.object )
					{
						findOuter = outerImport.object;
					}
				}

				CObject*	findObject = FindObjectFast( findClass, findOuter, importObject.objectName );

				// Reference to in memory-only package's object, native transient class
				bool		bIsInMemoryOnlyOrNativeTransient = bCameFromMemoryOnlyPackage || ( findObject && findObject->HasAllObjectFlags( OBJECT_Public | OBJECT_Native | OBJECT_Transient ) );
				if ( findObject && ( loadFlags & LOAD_FindIfFail || bIsInMemoryOnlyOrNativeTransient ) )
				{
					importObject.object = findObject;
					CObjectPackage::GetObjectSerializeContext().IncrementImportCount();
					CLinkerManager::Get().AddLoaderWithNewImports( this );
					return;
				}
			}
		}
	}
}

/*
==================
CLinkerLoad::VerifyImport
==================
*/
CLinkerLoad::EVerifyResult CLinkerLoad::VerifyImport( uint32 InImportIndex )
{
	// Map the object into our table
	ObjectImport&	importObject = importMap[InImportIndex];

	// Try to load the object
	VerifyImportInner( InImportIndex );

	// By default, we haven't failed yet
	EVerifyResult	result = VERIFY_Success;

	// Checks find out if the VerifyImportInner was successful or not 
	if ( importObject.sourceLinker && importObject.sourceIndex == INDEX_NONE && !importObject.object && !importObject.outerIndex.IsNull() && importObject.objectName != NAME_CObjectPackage )
	{
		// If we found the package, but not the object, look for a redirector
		ObjectImport	originalImport	= importObject;
		importObject.className			= NAME_CObjectRedirector;
		importObject.classPackage		= NAME_Core;

		// Try again for the redirector
		VerifyImportInner( InImportIndex );

		// If the redirector wasn't found, then it truly doesn't exist
		if ( importObject.sourceIndex == INDEX_NONE )
		{
			result = VERIFY_Failed;
		}
		// Otherwise we found that the redirector exists
		else
		{
			// Create the redirector (no serialization yet)
			CObjectRedirector*		objectRedirector = Cast<CObjectRedirector>( importObject.sourceLinker->CreateExport( importObject.sourceIndex ) );
			if ( !objectRedirector )
			{
				result = VERIFY_Failed;
			}
			else
			{
				// Serialize in the properties of the redirector (to get the object the redirector point to)
				// Always load redirectors. This will allow inner redirector references to always serialize fully here before accessing the GetDestinationObject()
				// and will be right linker with linker index
				objectRedirector->SetObjectFlags( OBJECT_NeedLoad );
				Preload( objectRedirector );
				CObject*	destinationObject = objectRedirector->GetDestinationObject();

				// Check to make sure the destination object was loaded
				if ( !destinationObject )
				{
					result = VERIFY_Failed;
				}
				else
				{
					// Check that in fact it was the type we thought it should be
					bool	bValidClass = false;
					CClass*	checkClass = destinationObject->GetClass();
					while ( !bValidClass && checkClass )
					{
						if ( checkClass->GetCName() == originalImport.className )
						{
							bValidClass = true;
							break;
						}
						checkClass = checkClass->GetSuperClass();
					}

					if ( !bValidClass )
					{
						// If the destination is a CObjectRedirector you've most likely made a nasty circular loop
						if ( destinationObject->GetClass() == CObjectRedirector::StaticClass() )
						{
							const std::wstring		outerName = importObject.outerIndex.IsNull() ? linkerRoot->GetFullName() : GetFullImportExportName( importObject.outerIndex );
							Warnf( TEXT( "VerifyImport: Import '%d' named '%s' in '%s' has circular loop of object redirection\n" ), InImportIndex, originalImport.objectName.ToString().c_str(), outerName.c_str() );
						}

						result = VERIFY_Failed;
					}
					else
					{
						// Broadcast that we followed a redirector successfully
						CObjectRedirector::onObjectRedirectorFollowed.Broadcast( filename, objectRedirector );

						// Now, fake our Import to be what the redirector pointed to
						result				= VERIFY_Redirected;
						importObject.object = destinationObject;
						CObjectPackage::GetObjectSerializeContext().IncrementImportCount();
						CLinkerManager::Get().AddLoaderWithNewImports( this );
					}
				}
			}
		}

		// Fix up the import. We put the original data back for the ClassName and ClassPackage (which are read off disk, and are expected not to change)
		importObject.className		= originalImport.className;
		importObject.classPackage	= originalImport.classPackage;

		// If nothing above failed, then we are good to go
		if ( result != VERIFY_Failed )
		{
			// We update the runtime information (SourceIndex, SourceLinker) to point to the object the redirector pointed to
			importObject.sourceIndex	= importObject.object->GetLinkerIndex();
			importObject.sourceLinker	= importObject.object->GetLinker();
		}
		else
		{
			// Put us back the way we were and peace out
			importObject = originalImport;
		}
	}

	return result;
}

/*
==================
CLinkerLoad::Preload
==================
*/
void CLinkerLoad::Preload( CObject* InObject )
{
	Assert( InObject );

	// Preload the object if necessary
	if ( InObject->HasAnyObjectFlags( OBJECT_NeedLoad ) )
	{
		if ( InObject->GetLinker() == this )
		{
			// If this is a struct, make sure that its parent struct is completely loaded
			if ( CStruct* theStruct = Cast<CStruct>( InObject ) )
			{
				if ( theStruct->GetSuperStruct() )
				{
					Preload( theStruct->GetSuperStruct() );
				}
			}

			// Make sure this object didn't get loaded in the above Preload call
			if ( InObject->HasAnyObjectFlags( OBJECT_NeedLoad ) )
			{
				// Grab the resource for this Object
				const uint32	exportIndex = InObject->GetLinkerIndex();
				ObjectExport&	exportObject = exportMap[exportIndex];
				Assert( exportObject.object == InObject );

				// Remember current position in the file
				const uint32	savedPos = loader->Tell();

				// Move to the position in the file where this object's data is stored
				Seek( exportObject.serialOffset );

				// Mark the object to indicate that it has been loaded
				InObject->RemoveObjectFlag( OBJECT_NeedLoad );

				// Serialize object's data
				InObject->Serialize( *this );

				// Make sure we serialized the right amount of stuff
				uint32		sizeSerialized = Tell() - exportObject.serialOffset;
				if ( sizeSerialized != exportObject.serialSize )
				{
					if ( InObject->GetClass()->HasAnyClassFlags( CLASS_Deprecated ) )
					{
						Warnf( TEXT( "%s: Serial size mismatch: Got %i, Expected %i\n" ), InObject->GetFullName().c_str(), sizeSerialized, exportObject.serialSize );
					}
					else
					{
						Sys_Error( TEXT( "%s: Serial size mismatch: Got %i, Expected %i" ), InObject->GetFullName().c_str(), sizeSerialized, exportObject.serialSize );
					}
				}

				// Restore original position in the file
				Seek( savedPos );
			}
		}
		else if ( CLinkerLoad* linker = InObject->GetLinker() )
		{
			// Send to the object's linker
			linker->Preload( InObject );
		}
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
		Sys_Error( TEXT( "Linker object %s %s.%s is invalid" ), GetExportClassName( InExportIndex ).ToString().c_str(), linkerRoot->GetName().c_str(), exportObject.objectName.ToString().c_str() );
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
	CLinkerManager::Get().RemoveLoaderFromObjectLoadersAndLoadersWithNewImports( this );
	CObjectPackage::GetObjectSerializeContext().RemoveDelayedLinkerClosePackage( this );

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
	CPackageIndex	index;
	*this << index;

	InValue = IndexToObject( index );
	return *this;
}