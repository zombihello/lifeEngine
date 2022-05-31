#include "Misc/Class.h"
#include "Misc/Object.h"

IMPLEMENT_CLASS( CObject )

/**
 * Serialize object
 */
void CObject::Serialize( CArchive& InArchive )
{
	InArchive << name;
}

/**
 * Whether the object is of the specified class
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