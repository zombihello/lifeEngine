#include "Reflection/Class.h"
#include "Reflection/Struct.h"
#include "Reflection/Property.h"

IMPLEMENT_CLASS( CStruct )

/*
==================
CStruct::StaticInitializeClass
==================
*/
void CStruct::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CStruct, superStruct ) );
	theClass->EmitObjectArrayReference( STRUCT_OFFSET( CStruct, properties ) );
}

/*
==================
CStruct::Serialize
==================
*/
void CStruct::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << propertiesSize;
	InArchive << minAlignment;
	InArchive << superStruct;
	InArchive << properties;
}

/*
==================
CStruct::InternalFindProperty
==================
*/
CProperty* CStruct::InternalFindProperty( const CName& InName, bool InFindInParents /* = true */ ) const
{
	for ( const CStruct* tempStruct = this; tempStruct; tempStruct = InFindInParents ? tempStruct->superStruct : nullptr )
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