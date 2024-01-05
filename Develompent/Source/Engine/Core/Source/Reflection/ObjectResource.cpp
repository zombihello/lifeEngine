#include "Reflection/ObjectResource.h"
#include "Reflection/Class.h"

/*
==================
ObjectResource::ObjectResource
==================
*/
ObjectResource::ObjectResource()
{}

/*
==================
ObjectResource::ObjectResource
==================
*/
ObjectResource::ObjectResource( CObject* InObject )
	: objectName( InObject ? InObject->GetCName() : NAME_None )
{}


/*
==================
ObjectExport::ObjectExport
==================
*/
ObjectExport::ObjectExport()
	: ObjectResource()
	, serialSize( 0 )
	, serialOffset( 0 )
	, object( nullptr )
{}

/*
==================
ObjectExport::ObjectExport
==================
*/
ObjectExport::ObjectExport( CObject* InObject )
	: ObjectResource( InObject )
	, serialSize( 0 )
	, serialOffset( 0 )
	, object( InObject )
{}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, ObjectExport& InValue )
{
	InArchive << InValue.classIndex;
	InArchive << InValue.superIndex;
	InArchive << InValue.outerIndex;
	InArchive << InValue.objectName;
	InArchive << InValue.serialSize;
	InArchive << InValue.serialOffset;
	InArchive << InValue.packageGuid;
	return InArchive;
}


/*
==================
ObjectImport::ObjectImport
==================
*/
ObjectImport::ObjectImport()
	: ObjectResource()
	, sourceIndex( INDEX_NONE )
	, object( nullptr )
	, sourceLinker( nullptr )
{}

/*
==================
ObjectImport::ObjectImport
==================
*/
ObjectImport::ObjectImport( CObject* InObject )
	: ObjectResource( InObject )
	, classPackage( InObject ? InObject->GetClass()->GetOuter()->GetCName() : NAME_None )
	, className( InObject ? InObject->GetClass()->GetCName() : NAME_None )
	, sourceIndex( INDEX_NONE )
	, object( InObject )
	, sourceLinker( nullptr )
{}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, ObjectImport& InValue )
{
	InArchive << InValue.classPackage;
	InArchive << InValue.className;
	InArchive << InValue.objectName;
	if ( InArchive.IsLoading() )
	{
		InValue.sourceLinker	= nullptr;
		InValue.sourceIndex		= INDEX_NONE;
		InValue.object			= nullptr;
	}

	return InArchive;
}