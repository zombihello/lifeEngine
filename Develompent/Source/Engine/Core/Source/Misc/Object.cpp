#include "Misc/Class.h"
#include "Misc/Object.h"

IMPLEMENT_CLASS( CObject )

BEGIN_DATADESC_NO_BASE( CObject )
END_DATADESC()

/*
==================
CObject::Serialize
==================
*/
void CObject::Serialize( CArchive& InArchive )
{
	InArchive << name;
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