#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/Class.h"
#include "Reflection/ReflectionEnvironment.h"
#include "System/BaseFileSystem.h"

/*
==================
CObjectPackage::ObjectExport::GetObject
==================
*/
CObject* CObjectPackage::ObjectExport::GetObject()
{
	// If object is exist return it
	if ( object )
	{
		return object;
	}

	// Get class of the object
	CReflectionEnvironment&		reflectionEnvironment = CReflectionEnvironment::Get();
	CClass*						theClass = className == NAME_None ? CClass::StaticClass() : reflectionEnvironment.FindClass( className.ToString().c_str() );
	if ( theClass == CClass::StaticClass() )
	{
		object = reflectionEnvironment.FindClass( objectName.ToString().c_str() );
	}

	// If object isn't valid yet then create a new
	if ( !object )
	{
		object = CObject::StaticConstructObject( theClass, nullptr, objectName, OBJECT_NeedDestroy );
	}
	return object;
}

/*
==================
CObjectPackage::CObjectPackage
==================
*/
CObjectPackage::CObjectPackage()
	: CArchive( TEXT( "" ) )
	, originalArchive( nullptr )
{
	arType = AT_Scripts;
}

/*
==================
CObjectPackage::~CObjectPackage
==================
*/
CObjectPackage::~CObjectPackage()
{
	RemoveAllObjects();
	if ( originalArchive )
	{
		delete originalArchive;
	}
}

/*
==================
CObjectPackage::AddObject
==================
*/
uint32 CObjectPackage::AddObject( CObject* InObject )
{
	// If object export for the object already created then return it's ID
	auto	it = objectExportsMap.find( InObject );
	if ( it != objectExportsMap.end() )
	{
		return it->second;
	}

	// Look for is need save the object. If not we save only info about class
	bool	bNeedSaveObject = false;
	for ( CObject* object = InObject; object; object = object->GetOuter() )
	{
		if ( object->HasAnyObjectFlags( OBJECT_NeedSave ) )
		{
			bNeedSaveObject = true;
			break;
		}
	}

	// Otherwise we create a new object export
	ObjectExport*	objectExport	= new ObjectExport( bNeedSaveObject ? InObject : nullptr );
	CClass*			theClass		= InObject->GetClass();
	CClass*			superClass		= theClass->GetSuperClass();
	objectExport->className			= theClass == CClass::StaticClass() ? NAME_None : theClass->GetCName();
	objectExport->superClassName	= superClass == CObject::StaticClass() ? NAME_None : superClass->GetCName();
	objectExport->objectName		= InObject->GetCName();

	// Add the object export into the package
	uint32			objectExportId = objectExports.size();
	objectExports.push_back( objectExport );
	objectExportsMap[InObject] = objectExportId;
	return objectExportId;
}

/*
==================
CObjectPackage::RemoveAllObjects
==================
*/
void CObjectPackage::RemoveAllObjects()
{
	for ( uint32 index = 0, count = objectExports.size(); index < count; ++index )
	{
		delete objectExports[index];
	}

	objectExportsMap.clear();
	objectExports.clear();
}

/*
==================
CObjectPackage::Load
==================
*/
bool CObjectPackage::Load( const std::wstring& InPath )
{
	originalArchive = g_FileSystem->CreateFileReader( InPath );
	if ( !originalArchive )
	{
		Warnf( TEXT( "Failed to load object package in '%s'\n" ), InPath.c_str() );
		return false;
	}
	arPath = InPath;

	// Serialize archive header
	originalArchive->SerializeHeader();
	Assert( originalArchive->Type() == AT_Scripts );

	// Serialize export map
	uint32		offsetToExportMap = 0;
	uint32		numSize = 0;
	uint32		offsetToPackageStart = originalArchive->Tell();
	*originalArchive << offsetToExportMap;
	originalArchive->Seek( offsetToExportMap );

	*originalArchive << numSize;
	objectExports.resize( numSize );
	for ( uint32 index = 0; index < numSize; ++index )
	{
		ObjectExport*	objectExport = new ObjectExport();
		*originalArchive << objectExport->className;
		*originalArchive << objectExport->superClassName;
		*originalArchive << objectExport->objectName;
		*originalArchive << objectExport->dataSize;
		objectExports[index] = objectExport;
	}

	// Serialize internal object's data
	originalArchive->Seek( offsetToPackageStart + sizeof( offsetToExportMap ) );
	for ( uint32 index = 0; index < numSize; ++index )
	{
		ObjectExport*	objectExport = objectExports[index];
		if ( objectExport->dataSize > 0 )
		{	
			bool		bNeedUpdateMap = !objectExport->IsValidObject();
			CObject*	object = objectExport->GetObject();
			uint32		dataOffset = originalArchive->Tell();
			
			object->Serialize( *this );
			Assert( originalArchive->Tell() - dataOffset == objectExport->dataSize );

			if ( bNeedUpdateMap )
			{
				objectExportsMap[object] = index;
			}
		}
	}

	// Delete archive and exit
	delete originalArchive;
	originalArchive = nullptr;
	return true;
}

/*
==================
CObjectPackage::Save
==================
*/
bool CObjectPackage::Save( const std::wstring& InPath )
{
	originalArchive		= g_FileSystem->CreateFileWriter( InPath );
	if ( !originalArchive )
	{
		Warnf( TEXT( "Failed to save object package in '%s'\n" ), InPath.c_str() );
		return false;
	}
	arPath = InPath;

	// Set archive type and serialize it's header
	originalArchive->SetType( AT_Scripts );
	originalArchive->SerializeHeader();

	// Rest place for offset to export map in the package
	uint32		offsetToExportMap = 0;
	uint32		offsetToPackageStart = originalArchive->Tell();
	*originalArchive << offsetToExportMap;

	// Serialize all objects
	for ( uint32 index = 0; index < objectExports.size(); ++index )
	{
		// Serialize object export and internal data
		ObjectExport*		objectExport = objectExports[index];
		CObject*			object = objectExport->GetObject();
		if ( object )
		{
			uint32			dataOffset = originalArchive->Tell();
			object->Serialize( *this );
			object->RemoveObjectFlag( OBJECT_NeedSave );
			objectExport->dataSize = originalArchive->Tell() - dataOffset;
		}	
	}

	// Serialize export map
	offsetToExportMap = originalArchive->Tell();
	uint32		numSize = objectExports.size();
	*originalArchive << numSize;
	for ( uint32 index = 0; index < numSize; ++index )
	{
		ObjectExport*	objectExport = objectExports[index];
		*originalArchive << objectExport->className;
		*originalArchive << objectExport->superClassName;
		*originalArchive << objectExport->objectName;
		*originalArchive << objectExport->dataSize;
	}

	// Update offset to export map
	originalArchive->Seek( offsetToPackageStart );
	*originalArchive << offsetToExportMap;

	// Delete archive and exit
	delete originalArchive;
	originalArchive = nullptr;
	return true;
}

/*
==================
CObjectPackage::operator<<
==================
*/
CArchive& CObjectPackage::operator<<( class CObject*& InValue )
{
	// By default is invalid object export ID
	uint32		objectExportId = INVALID_ID;
	CArchive&	archive = *this;

	// If we save the object and it isn't exist a object export in the package
	// then we create a new and add into package
	if ( originalArchive->IsSaving() && InValue )
	{
		auto	it = objectExportsMap.find( InValue );
		if ( it != objectExportsMap.end() )
		{
			objectExportId = it->second;
		}
		else
		{ 
			objectExportId = AddObject( InValue );
		}
	}

	// Serialize the object export ID
	archive << objectExportId;

	// If we load the object and its NULL in the object export
	// then we have to create a new object. 
	// When object export ID is INVALID_ID its mean what CObject was NULL
	if ( originalArchive->IsLoading() && objectExportId != INVALID_ID )
	{
		ObjectExport*	objectExport	= objectExports[objectExportId];
		bool			bNeedUpdateMap	= !objectExport->IsValidObject();
		InValue = objectExport->GetObject();
		if ( bNeedUpdateMap )
		{
			objectExportsMap[InValue] = objectExportId;
		}
	}

	return archive;
}

/*
==================
CObjectPackage::Serialize
==================
*/
void CObjectPackage::Serialize( void* InBuffer, uint32 InSize )
{
	originalArchive->Serialize( InBuffer, InSize );
}

/*
==================
CObjectPackage::Tell
==================
*/
uint32 CObjectPackage::Tell()
{
	return originalArchive ? originalArchive->Tell() : 0;
}

/*
==================
CObjectPackage::Seek
==================
*/
void CObjectPackage::Seek( uint32 InPosition )
{
	if ( originalArchive )
	{
		originalArchive->Seek( InPosition );
	}
}

/*
==================
CObjectPackage::Flush
==================
*/
void CObjectPackage::Flush()
{
	if ( originalArchive )
	{
		originalArchive->Flush();
	}
}

/*
==================
CObjectPackage::IsSaving
==================
*/
bool CObjectPackage::IsSaving() const
{
	return originalArchive ? originalArchive->IsSaving() : false;
}

/*
==================
CObjectPackage::IsLoading
==================
*/
bool CObjectPackage::IsLoading() const
{
	return originalArchive ? originalArchive->IsLoading() : false;
}

/*
==================
CObjectPackage::IsEndOfFile
==================
*/
bool CObjectPackage::IsEndOfFile()
{
	return originalArchive ? originalArchive->IsEndOfFile() : false;
}

/*
==================
CObjectPackage::GetSize
==================
*/
uint32 CObjectPackage::GetSize()
{
	return originalArchive ? originalArchive->GetSize() : 0;
}