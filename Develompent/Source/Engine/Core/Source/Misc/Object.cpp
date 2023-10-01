#include "Misc/Class.h"
#include "Misc/Object.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CObject )

/*
==================
CObject::CObject
==================
*/
CObject::CObject()
	: name( NoInit )
{}

/*
==================
CObject::CObject
==================
*/
CObject::CObject( const CName& InName )
	: name( InName )
	, outer( nullptr )
{}

/*
==================
CObject::Serialize
==================
*/
void CObject::Serialize( CArchive& InArchive )
{
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_CObjectHasCName )
	{
		std::wstring	tempName;
		InArchive << tempName;
		name = tempName;
		Warnf( TEXT( "Deprecated package version (0x%X). Need to re-save the package '%s', because in the future it may not open\n" ), InArchive.Ver(), InArchive.GetPath().c_str() );
	}
	else
	{
		InArchive << name;
	}
}

/*
==================
CObject::StaticAllocateObject
==================
*/
CObject* CObject::StaticAllocateObject( class CClass* InClass, CObject* InOuter /* = nullptr */, CName InName /* = NAME_None */ )
{
	AssertMsg( IsInGameThread(), TEXT( "Can't create %s/%s while outside of game thread" ), InClass->GetName().c_str(), InName.ToString().c_str() );
	if ( !InClass )
	{
		Sys_Errorf( TEXT( "Empty class for object %s\n" ), InName.ToString().c_str() );
		return nullptr;
	}

	// If we try to create abstract object it's wrong and we call error
	if ( InClass->HasAnyClassFlags( CLASS_Abstract ) )
	{
		Sys_Errorf( TEXT( "Class which was marked abstract was trying to be allocated. %s/%s" ), InClass->GetName().c_str(), InName.ToString().c_str() );
		return nullptr;
	}

	// Compose name, if unnamed
	if ( InName == NAME_None )
	{
		InName = InClass->GetCName();
	}

	// Allocated data for a new object
	uint32		alignedSize = Align( InClass->GetPropertiesSize(), InClass->GetMinAlignment() );
	CObject*	object = ( CObject* )malloc( alignedSize );
	Sys_Memzero( ( void* )object, InClass->GetPropertiesSize() );

	// Init object properties
	object->outer		= InOuter;
	object->name		= InName;
	return object;
}

/*
==================
CObject::StaticConstructObject
==================
*/
CObject* CObject::StaticConstructObject( class CClass* InClass, CObject* InOuter /* = nullptr */, CName InName /* = NAME_None */ )
{
	// Allocate a new object and call the class constructor
	CObject*	object = StaticAllocateObject( InClass, InOuter, InName );
	if ( object )
	{
		InClass->ClassConstructor( object );
	}

	return object;
}

/*
==================
CObject::InternalIsA
==================
*/
bool CObject::InternalIsA( const CClass* InClass ) const
{
	if ( !InClass )
	{
		return false;
	}

	for ( const CClass* tempClass = GetClass(); tempClass; tempClass = tempClass->GetSuperClass() )
	{
		if ( tempClass == InClass )
		{
			return true;
		}
	}

	return false;
}

/*
==================
CObject::StaticInitializeClass
==================
*/
void CObject::StaticInitializeClass()
{}

#if WITH_EDITOR
/*
==================
CObject::PostEditChangeProperty
==================
*/
void CObject::PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet )
{}

/*
==================
CObject::CanEditProperty
==================
*/
bool CObject::CanEditProperty( class CProperty* InProperty ) const
{
	return true;
}
#endif // WITH_EDITOR

/*
==================
CObject::AddProperty
==================
*/
void CObject::AddProperty( class CProperty* InProperty )
{
	Sys_Errorf( TEXT( "CObject::AddProperty: Not implemented" ) );
}