#include "Misc/Class.h"
#include "Misc/Object.h"

IMPLEMENT_CLASS( LObject )

/**
 * Serialize object
 */
void LObject::Serialize( FArchive& InArchive )
{
	InArchive << name;
}

/**
 * Whether the object is of the specified class
 */
bool LObject::InternalIsA( const LClass* InClass ) const
{
	if ( !InClass )
	{
		return false;
	}

	for ( const LClass* tempClass = GetClass(); tempClass; tempClass = tempClass->GetSuperClass() )
	{
		if ( tempClass == InClass )
		{
			return true;
		}
	}

	return false;
}