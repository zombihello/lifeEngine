#include "Misc/JsonDocument.h"
#include "Misc/StringConv.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectGlobals.h"
#include "System/ObjectImporter.h"
#include "System/World.h"
#include "Actors/Actor.h"

/*
==================
CObjectImporter::CObjectImporter
==================
*/
CObjectImporter::CObjectImporter()
	: portFlags( PPF_None )
	, importRootScope( nullptr )
{}

/*
==================
CObjectImporter::Import
==================
*/
void CObjectImporter::Import( const std::wstring& InString, std::vector<CObject*>& OutObjects, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	CJsonDocument	jsonDocument;
	bool			bResult = jsonDocument.LoadFromBuffer( TCHAR_TO_ANSI( InString.c_str() ) );
	if ( !bResult )
	{
		Warnf( TEXT( "Failed to import objects, maybe wrong JSON syntax\n" ) );
		return;
	}

	// Set import parameters
	portFlags = InPortFlags;
	importRootScope = InImportRootScope;
	OutObjects.clear();
	objectDict.clear();

	// Get 'ExportedObjects' section
	const CJsonValue*					jsonExportedObjects = jsonDocument.GetValue( TEXT( "ExportedObjects" ) );
	if ( !jsonExportedObjects )
	{
		Warnf( TEXT( "Failed to import objects, section 'ExportedObjects' is missing\n" ) );
		return;
	}
	const std::vector<CJsonValue>*		exportedObjects = jsonExportedObjects->GetArray();
	if ( !exportedObjects )
	{
		Warnf( TEXT( "Failed to import objects, section 'ExportedObjects' has wrong type (must be array)\n" ) );
		return;
	}

	// Import each object
	for ( uint32 objectIdx = 0, numObjects = exportedObjects->size(); objectIdx < numObjects; ++objectIdx )
	{
		const CJsonValue&		jsonArrayElement = exportedObjects->at( objectIdx );
		const CJsonObject*		jsonObject = jsonArrayElement.GetObject();
		if ( !jsonObject )
		{
			Warnf( TEXT( "Failed to import object %i, invalid object\n" ), objectIdx );
			continue;
		}

		// Import object
		CObject*	object = nullptr;
		objectDict.clear();
		ImportObject( jsonObject, object, importRootScope, false );

		// If failed to import an object, skip it
		if ( !object )
		{
			Warnf( TEXT( "Failed to import object %i\n" ), objectIdx );
			continue;
		}

		// Otherwise save it and go to next object
		Logf( TEXT( "Imported object '%s'\n" ), object->GetFullName().c_str() );
		OutObjects.push_back( object );
	}
}

/*
==================
CObjectImporter::ImportObject
==================
*/
void CObjectImporter::ImportObject( const CJsonObject* InJsonObject, CObject*& OutObject, CObject* InOuter, bool InIsSubobject )
{
	// Get object class from JSON
	Assert( InJsonObject );
	const CJsonValue*	jsonObjectClass = InJsonObject->GetValue( TEXT( "Class" ) );
	std::wstring		strObjectClass = jsonObjectClass ? jsonObjectClass->GetString() : TEXT( "" );
	OutObject			= nullptr;

	// Find object class and construct the one
	CClass*		objectClass = FindObject<CClass>( ANY_PACKAGE, strObjectClass.c_str(), true );
	if ( !objectClass )
	{
		Warnf( TEXT( "ImportObject: Object class '%s' not found\n" ), strObjectClass.c_str() );
		return;
	}

	// Get object name from JSON and make unique name
	const CJsonValue*	jsonObjectName = InJsonObject->GetValue( TEXT( "Name" ) );
	if ( !jsonObjectName )
	{
		Warnf( TEXT( "ImportObject: Object name isn't valid\n" ) );
		return;
	}

	// Make unique object name
	CName	sourceObjectName = jsonObjectName->GetString();
	CName	uniqueObjectName = sourceObjectName;
	
	// Find already created object in memory
	auto	itObject = objectDict.find( uniqueObjectName );
	if ( itObject != objectDict.end() )
	{
		OutObject = itObject->second;
	}
	// Otherwise construct a new object
	else
	{
		uniqueObjectName = CObject::MakeUniqueObjectName( InOuter, objectClass, sourceObjectName.ToString() );
		if ( objectClass->IsChildOf( AActor::StaticClass() ) && IsA<CWorld>( InOuter ) )
		{
			// Spawn an actor by CWorld::SpawnActor if objectClass is child of AActor and InOuter is a CWorld
			CWorld*		world = Cast<CWorld>( InOuter );
			OutObject	= world->SpawnActor( objectClass, Math::vectorZero, Math::rotatorZero, uniqueObjectName );

		}
		// Otherwise create object by NewObject
		else
		{
			OutObject = NewObject( objectClass, InOuter, uniqueObjectName );
		}
	}
	Assert( OutObject );

	// Grab all subobjects that have been created in constructor
	std::vector<CObject*>							subObjects;
	TArchiveObjectReferenceCollector<CObject>		objectReferenceCollector( &subObjects, OutObject );
	OutObject->Serialize( objectReferenceCollector );
	for ( uint32 index = 0, count = subObjects.size(); index < count; ++index )
	{
		CObject*	subObject = subObjects[index];
		objectDict.insert( std::make_pair( subObject->GetCName(), subObject ) );
	}

	// Import subobjects
	TGuardValue<CObject*>			guard_ImportRootScope( importRootScope, !InIsSubobject ? OutObject : importRootScope );
	const CJsonValue*				jsonSubObjectsValue = InJsonObject->GetValue( TEXT( "SubObjects" ) );
	const std::vector<CJsonValue>*	jsonSubObjects = jsonSubObjectsValue ? jsonSubObjectsValue->GetArray() : nullptr;
	if ( jsonSubObjects )
	{
		for ( uint32 index = 0, count = jsonSubObjects->size(); index < count; ++index )
		{
			const CJsonValue&	jsonSubObjectElement	= jsonSubObjects->at( index );
			const CJsonObject*	jsonSubObject			= jsonSubObjectElement.GetObject();
			if ( !jsonSubObject )
			{
				Warnf( TEXT( "ImportObject: Failed to import subobject %i at '%s'\n" ), index, OutObject->GetFullName().c_str() );
				continue;
			}

			// Import subobject
			CObject*	subObject = nullptr;
			ImportObject( jsonSubObject, subObject, OutObject, true );

			// If failed to import an subobject, skip it
			if ( !subObject )
			{
				Warnf( TEXT( "ImportObject: Failed to import subobject %i at '%s'\n" ), index, OutObject->GetFullName().c_str() );
				continue;
			}
			Logf( TEXT( "ImportObject: Imported subobject '%s'\n" ), subObject->GetFullName().c_str() );
		}
	}
	else
	{
		Warnf( TEXT( "ImportObject: Object '%s' has invalid JSON section 'SubObjects'\n" ), OutObject->GetFullName().c_str() );
	}

	// Import object data
	const CJsonValue*		jsonDataValue = InJsonObject->GetValue( TEXT( "Data" ) );
	const CJsonObject*		jsonData = jsonDataValue ? jsonDataValue->GetObject() : nullptr;
	if ( jsonData )
	{
		std::vector<CProperty*>		properties;
		OutObject->GetClass()->GetProperties( properties );
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			CProperty*			property = properties[index];
			const CJsonValue*	jsonProperty = jsonData->GetValue( property->GetName().c_str() );
			if ( !jsonProperty )
			{
				Warnf( TEXT( "ImportObject: Property '%s' not found in the JSON section 'Data' of '%s'\n" ), property->GetName().c_str(), OutObject->GetFullName().c_str() );
				continue;
			}

			property->ImportProperty( jsonProperty, ( byte* )OutObject, importRootScope, portFlags );
		}
	}
	else
	{
		Warnf( TEXT( "ImportObject: Object '%s' has invalid JSON section 'Data'\n" ), OutObject->GetFullName().c_str() );
	}
}