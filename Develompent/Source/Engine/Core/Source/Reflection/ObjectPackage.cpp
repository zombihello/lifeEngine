#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "Misc/Template.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/Linker.h"
#include "Reflection/Class.h"
#include "Reflection/ReflectionEnvironment.h"
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
	 * @param InIsRoutePresave	Is need call PreSave()
	 */
	void TagPackageExports( CArchiveSaveTagExports& InExportTagger, bool InIsRoutePresave )
	{
		// Route PreSave on Base and serialize it for export tagging
		if ( base )
		{
			if ( InIsRoutePresave )
			{
				base->PreSave();
			}

			InExportTagger.ProcessBaseObject( base );
		}

		// Serialize objects to tag them as OBJECT_TagExp
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( topLevelFlags ) || IsIn( object, outer ) )
			{
				InExportTagger.ProcessBaseObject( object );
			}
		}

		if ( InIsRoutePresave )
		{
			// Route PreSave
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
{}

/*
==================
CObject::CreatePackage
==================
*/
CObjectPackage* CObjectPackage::CreatePackage( CObject* InOuter, const tchar* InPackageName )
{
	std::wstring	name;
	if ( InPackageName )
	{
		name = InPackageName;
	}

	// TODO yehor.pohuliaka: Need optimize find a package
	for ( TObjectIterator<CObjectPackage> it; it; ++it )
	{
		if ( it->GetOuter() == InOuter && it->GetName() == name )
		{
			return *it;
		}
	}

	return new( InOuter, name ) CObjectPackage;
}

/*
==================
CObject::LoadPackage
==================
*/
CObjectPackage* CObjectPackage::LoadPackage( CObjectPackage* InOuter, const tchar* InFilename )
{
	AssertNoEntry();
	return nullptr;
}

/*
==================
CObject::SavePackage
==================
*/
bool CObjectPackage::SavePackage( CObjectPackage* InOuter, CObject* InBase, ObjectFlags_t InTopLevelFlags, const tchar* InFilename )
{
	// Check on recursive call SavePackage, it's error
	if ( bIsSavingPackage )
	{
		Warnf( TEXT( "Recursive CObjectPackage::SavePackage() is not supported\n" ) );
		AssertNoEntry();
		return false;
	}

	// Make temp file
	double			timeStart = Sys_Seconds();
	std::wstring	baseFilename = CFilename( InFilename ).GetBaseFilename();
	CFilename		tempFilename = CFilename( InFilename ).GetPath() + PATH_SEPARATOR + baseFilename + TEXT( "_save.tmp" );
	bool			bSuccess = false;

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

	// Tag exports and route PreSave
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

		// We need to serialize objects yet again to tag objects that were created by PreSave as OBJECT_TagExp
		packageExportTagger.TagPackageExports( exportTaggerArchive, false );

		// Allocate the linker
		CLinkerSave		linker( InOuter, tempFilename.GetFullPath().c_str() );

		// Import objects
		for ( CObjectIterator it; it; ++it )
		{
			CObject*	object = *it;
			if ( object->HasAnyObjectFlags( OBJECT_TagExp ) )
			{
				CArchiveSaveTagImports	importTagger( &linker );
				CClass*					theClass = object->GetClass();

				object->Serialize( importTagger );
				importTagger << theClass;
			}
		}

		// Generate a new guid for the package
		linker.GetSummary().guid = Sys_CreateGuid();

		// Make sure the CPackage's copy of the GUID is up to date
		InOuter->guid = linker.GetSummary().guid;

		// Rest place for package summary, we update it in the end
		linker << linker.GetSummary();
		uint32		offsetAfterPackageFileSummary = linker.Tell();

		// Build import map
		std::vector<ObjectImport>&	importMap = linker.GetImports();
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
		// also set required data for any CPackages in the export map
		std::vector<CPackageIndex>&		objectIndeces = linker.GetObjectIndices();
		for ( uint32 index = 0, count = exportMap.size(); index < count; ++index )
		{
			CObject*	object = exportMap[index].object;
			if ( object )
			{
				objectIndeces[object->GetIndex()] = CPackageIndex::FromExport( index );
				CObjectPackage*		package = Cast<CObjectPackage>( object );
				if ( package )
				{
					exportMap[index].packageGuid = package->GetGuid();
				}
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
					AssertMsg( !objectExport.classIndex.IsNull(), TEXT( "Export %s class is not mapped when saving %s" ), *objectExport.object->GetName().c_str(), linker.GetLinkerRoot()->GetName().c_str() );
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
								   theStruct->GetName().c_str(),
								   theStruct->GetClass()->GetName().c_str(),
								   theStruct->GetSuperStruct()->GetName().c_str(),
								   theStruct->GetSuperStruct()->GetClass()->GetName().c_str() );
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

				// Set CPackageIndex for this export's Outer. If the export's Outer
				// is the CPackage corresponding to this package's LinkerRoot
				if ( objectExport.object->GetOuter() != InOuter )
				{
					Assert( objectExport.object->GetOuter() );
					objectExport.outerIndex = objectIndeces[objectExport.object->GetOuter()->GetIndex()];

					AssertMsg( IsIn( objectExport.object->GetOuter(), InOuter ),
							   TEXT( "Export Object (%s) Outer (%s) mismatch" ),
							   objectExport.object->GetName().c_str(),
							   objectExport.object->GetOuter()->GetName().c_str() );
					AssertMsg( !objectExport.outerIndex.IsImport(),
							   TEXT( "Export Object (%s) Outer (%s) is an Import" ),
							   objectExport.object->GetName().c_str(),
							   objectExport.object->GetOuter()->GetName().c_str() );
					AssertMsg( !objectExport.outerIndex.IsNull(),
							   TEXT( "Export Object (%s) Outer (%s) index is root package" ),
							   objectExport.object->GetName().c_str(),
							   objectExport.object->GetOuter()->GetName().c_str() );

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
				// Set the package index
				if ( objectImport.object->GetOuter() )
				{
					if ( IsIn( objectImport.object->GetOuter(), InOuter ) )
					{
						if ( !objectImport.object->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) )
						{
							Warnf( TEXT( "Bad object '%s'\n" ), objectImport.object->GetName().c_str() );
						}
						else
						{
							// If an object is marked OBJECT_Transient|OBJECT_Native, it is either an intrinsic class or
							// a property of an intrinsic class. Only properties of intrinsic classes will have
							// an Outer that passes the check for "IsIn( GetOuter, InOuter )" (thus ending up in this
							// block of code). Just verify that the Outer for this property is also marked OBJECT_Transient|OBJECT_Native
							Assert( objectImport.object->GetOuter()->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) );
						}
					}

					Assert( !IsIn( objectImport.object->GetOuter(), InOuter ) || objectImport.object->HasAllObjectFlags( OBJECT_Native | OBJECT_Transient ) );
					objectImport.outerIndex = objectIndeces[objectImport.object->GetOuter()->GetIndex()];
				}
	
				// Save it
				linker << objectImport;
			}
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
				InOuter->SetDirtyFlag( false );
			}

			// Delete the temporary file
			g_FileSystem->Delete( tempFilename.GetFullPath() );
		}
	}

	return bSuccess;
}