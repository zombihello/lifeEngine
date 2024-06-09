#include "Reflection/ObjectMacros.h"
#include "Reflection/ObjectResource.h"
#include "Reflection/Class.h"

/*
==================
ObjectResource::ObjectResource
==================
*/
ObjectResource::ObjectResource()
	: objectName( NAME_None )
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
	: objectFlags( 0 )
	, serialSize( 0 )
	, serialOffset( 0 )
	, object( nullptr )
	, hashNext( INDEX_NONE )
{}

/*
==================
ObjectExport::ObjectExport
==================
*/
ObjectExport::ObjectExport( CObject* InObject )
	: ObjectResource( InObject )
	, objectFlags( InObject ? ( InObject->GetObjectFlags() & OBJECT_MASK_Load ) : 0 )
	, serialSize( 0 )
	, serialOffset( 0 )
	, object( InObject )
	, hashNext( INDEX_NONE )
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
	InArchive << InValue.objectFlags;
	InArchive << InValue.serialSize;
	InArchive << InValue.serialOffset;
	return InArchive;
}


/*
==================
ObjectImport::ObjectImport
==================
*/
ObjectImport::ObjectImport()
	: sourceIndex( INDEX_NONE )
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
	InArchive << InValue.outerIndex;
	InArchive << InValue.objectName;
	if ( InArchive.IsLoading() )
	{
		InValue.sourceLinker	= nullptr;
		InValue.sourceIndex		= INDEX_NONE;
		InValue.object			= nullptr;
	}

	return InArchive;
}