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
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CObjectPackage )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObjectPackage )

//
// GLOBALS
//
bool		CObjectPackage::bIsSavingPackage = false;

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
bool CObjectPackage::SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename, uint32 InSaveFlags, CBaseTargetPlatform* InCookingTarget /* = nullptr */ )
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
		CLinkerSave		linker( InOuter, tempFilename.GetFullPath().c_str() );
		linker.SetFilterEditorOnly( bFilterEditorOnly );
		linker.SetCookingTarget( InCookingTarget );

		// Import objects
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( OBJECT_TagExp ) )
			{
				CArchiveSaveTagImports	importTagger( &linker );
				CClass*					theClass = object->GetClass();

				importTagger.SetFilterEditorOnly( bFilterEditorOnly );
				importTagger.SetCookingTarget( InCookingTarget );
				object->Serialize( importTagger );
				importTagger << theClass;
			}
		}

		// Save package flags
		linker.GetSummary().SetPackageFlags( InOuter->GetPackageFlags() | ( bIsCooking ? PKG_Cooked | PKG_FilterEditorOnly : 0 ) );

		// Rest place for package summary, we update it in the end
		linker << linker.GetSummary();
		uint32		offsetAfterPackageFileSummary = linker.Tell();

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
			// Move the temporary file
			Logf( TEXT( "Moving '%s' to '%s'\n" ), tempFilename.GetFullPath().c_str(), InFilename );
			bSuccess = g_FileSystem->Move( InFilename, tempFilename.GetFullPath(), true ) == CMR_OK;

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