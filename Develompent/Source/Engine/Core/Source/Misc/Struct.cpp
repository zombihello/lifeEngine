#include "Misc/Struct.h"
#include "Misc/Class.h"
#include "Misc/Property.h"

IMPLEMENT_CLASS( CStruct )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CStruct )

/*
==================
CStruct::InternalFindProperty
==================
*/
CProperty* CStruct::InternalFindProperty( const CName& InName ) const
{
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = tempStruct->superStruct )
	{
		for ( uint32 index = 0, count = tempStruct->properties.size(); index < count; ++index )
		{
			if ( tempStruct->properties[index]->GetCName() == InName )
			{
				return tempStruct->properties[index];
			}
		}
	}
	return nullptr;
}

/*
==================
CStruct::GetProperties
==================
*/
void CStruct::GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents /* = true */ ) const
{
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = InPropertiesInParents ? tempStruct->superStruct : nullptr )
	{
		if ( !tempStruct->properties.empty() )
		{
			uint32		offset = OutArrayProperties.size();
			OutArrayProperties.resize( tempStruct->properties.size() + offset );
			memcpy( OutArrayProperties.data() + offset, tempStruct->properties.data(), tempStruct->properties.size() * sizeof( CProperty* ) );
		}
	}
}

/*
==================
CStruct::GetNumProperties
==================
*/
uint32 CStruct::GetNumProperties( bool InPropertiesInParents /* = true */ ) const
{
	uint32		numProps = 0;
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = InPropertiesInParents ? tempStruct->superStruct : nullptr )
	{
		numProps += tempStruct->properties.size();
	}

	return numProps;
}

/*
==================
CStruct::AddProperty
==================
*/
void CStruct::AddProperty( CProperty* InProperty )
{
	properties.push_back( InProperty );
}