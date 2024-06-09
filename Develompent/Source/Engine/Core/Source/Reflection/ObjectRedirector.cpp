#include "Reflection/Class.h"
#include "Reflection/ObjectRedirector.h"

IMPLEMENT_CLASS( CObjectRedirector )

//
// GLOBALS
//
COnObjectRedirectorFollowed		CObjectRedirector::onObjectRedirectorFollowed;

/*
==================
CObjectRedirector::StaticInitializeClass
==================
*/
void CObjectRedirector::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CObjectRedirector, destinationObject ) );
}

/*
==================
CObjectRedirector::CObjectRedirector
==================
*/
CObjectRedirector::CObjectRedirector()
	: destinationObject( nullptr )
{}

/*
==================
CObjectRedirector::PreSave
==================
*/
void CObjectRedirector::PreSave()
{
	if ( !destinationObject || destinationObject->HasAnyObjectFlags( OBJECT_Transient ) )
	{
		SetObjectFlags( OBJECT_Transient );
	}
}

/*
==================
CObjectRedirector::Serialize
==================
*/
void CObjectRedirector::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << destinationObject;
}