#include "Misc/Class.h"
#include "Misc/Property.h"

// ----------------
// STATIC VALUES
// ----------------

std::unordered_map< std::wstring, const CClass* >			CClass::classesTable;

/*
==================
CClass::InternalFindProperty
==================
*/
CProperty* CClass::InternalFindProperty( const CName& InName ) const
{
	for ( const CClass* tempClass = this; tempClass; tempClass = tempClass->superClass )
	{
		for ( uint32 index = 0, count = tempClass->properties.size(); index < count; ++index )
		{
			if ( tempClass->properties[index]->GetCName() == InName )
			{
				return tempClass->properties[index];
			}
		}
	}
	return nullptr;
}

/*
==================
CClass::GetProperties
==================
*/
void CClass::GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents /* = true */ ) const
{
	for ( const CClass* tempClass = this; tempClass; tempClass = InPropertiesInParents ? tempClass->superClass : nullptr )
	{
		if ( !tempClass->properties.empty() )
		{
			uint32		offset = OutArrayProperties.size();
			OutArrayProperties.resize( tempClass->properties.size() + offset );
			memcpy( OutArrayProperties.data() + offset, tempClass->properties.data(), tempClass->properties.size() * sizeof( CProperty* ) );
		}
	}
}

/*
==================
CClass::GetNumProperties
==================
*/
uint32 CClass::GetNumProperties( bool InPropertiesInParents /* = true */ ) const
{
	uint32		numProps = 0;
	for ( const CClass* tempClass = this; tempClass; tempClass = InPropertiesInParents ? tempClass->superClass : nullptr )
	{
		numProps += tempClass->properties.size();
	}

	return numProps;
}