#include "Misc/Class.h"
#include "Misc/Object.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( CObject )

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
void CObject::PostEditChangeProperty( class CProperty* InProperty, EPropertyChangeType InChangeType )
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