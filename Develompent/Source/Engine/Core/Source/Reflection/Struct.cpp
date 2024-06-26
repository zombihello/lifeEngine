#include "Reflection/Class.h"
#include "Reflection/Struct.h"
#include "Reflection/Property.h"
#include "Reflection/FieldIterator.h"

IMPLEMENT_CLASS( CStruct )

/*
==================
CStruct::StaticInitializeClass
==================
*/
void CStruct::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CStruct, childrenField ) );
	theClass->EmitObjectReference( STRUCT_OFFSET( CStruct, superStruct ) );
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
	InArchive << childrenField;

	// Preload the super struct
	if ( superStruct )
	{
		InArchive.Preload( superStruct );
	}

	// Preload all fields
	for ( TFieldIterator<CField> it( this, false ); it; ++it )
	{
		InArchive.Preload( *it );
	}
}

/*
==================
CStruct::AddProperty
==================
*/
void CStruct::AddProperty( CProperty* InProperty )
{
	InProperty->SetNextField( childrenField );
	childrenField = InProperty;
}

/*
==================
CStruct::SerializeProperties
==================
*/
void CStruct::SerializeProperties( class CArchive& InArchive, byte* InData )
{
	if ( !InArchive.WantBinaryPropertySerialization() )
	{
		SerializeTaggedProperties( InArchive, InData );
	}
	else
	{
		SerializeBinaryProperties( InArchive, InData );
	}
}

/*
==================
CStruct::SerializeTaggedProperties
==================
*/
void CStruct::SerializeTaggedProperties( class CArchive& InArchive, byte* InData )
{
	std::vector<CPropertyTag>	serializedProperties;

	// If we saving archive then need fill the array
	if ( InArchive.IsSaving() )
	{
		for ( TFieldIterator<CProperty> it( this ); it; ++it )
		{
			CProperty*		property = *it;
			if ( property->ShouldSerializeValue( InArchive ) )
			{
				serializedProperties.push_back( CPropertyTag( property ) );
			}
		}
	}

	// Remember offset to the begin array of tags
	uint32		offsetSerializedProperties = InArchive.Tell();

	// Serialize tags
	InArchive << serializedProperties;

	// If we loading archive then need associate tags and our properties in the struct
	if ( InArchive.IsLoading() )
	{
		// Fill map for faster find
		std::unordered_map<uint64, CPropertyTag*>	tagsMap;
		for ( uint32 index = 0, count = serializedProperties.size(); index < count; ++index )
		{
			CPropertyTag*	propertyTag = &serializedProperties[index];
			uint64			hash = propertyTag->GetPropertyName().GetHash();
			FastHash( propertyTag->GetClassName().GetHash(), hash );
			tagsMap.insert( std::make_pair( hash, propertyTag ) );
		}

		// Associate tags with out properties in the struct
		for ( TFieldIterator<CProperty> it( this ); it; ++it )
		{
			CProperty*	property	= *it;
			uint64		hash		= property->GetCName().GetHash();
			FastHash( property->GetClass()->GetCName().GetHash(), hash );

			// Try find tag for the property
			auto	itTag = tagsMap.find( hash );
			if ( itTag != tagsMap.end() )
			{
				itTag->second->SetProperty( property );
			}
		}
	}

	// Serialize properties data
	if ( InArchive.IsSaving() )
	{
		for ( uint32 index = 0, count = serializedProperties.size(); index < count; ++index )
		{
			CPropertyTag&	propertyTag		= serializedProperties[index];
			CProperty*		property		= propertyTag.GetProperty();
			uint32			currentOffset	= InArchive.Tell();
			Assert( property );

			// Serialized property's data and remember size of this data
			property->SerializeProperty( InArchive, InData );
			propertyTag.SetSerialSize( InArchive.Tell() - currentOffset );
		}

		// Update tags
		if ( !serializedProperties.empty() )
		{
			uint32		currentOffset = InArchive.Tell();
			InArchive.Seek( offsetSerializedProperties );
			InArchive << serializedProperties;
			InArchive.Seek( currentOffset );
		}
	}
	else
	{
		for ( uint32 index = 0, count = serializedProperties.size(); index < count; ++index )
		{
			// If the property tag no have an associated property then we skip it
			CPropertyTag&	propertyTag		= serializedProperties[index];
			CProperty*		property		= propertyTag.GetProperty();
			uint32			currentOffset	= InArchive.Tell();
			if ( !property )
			{
				InArchive.Seek( currentOffset + propertyTag.GetSerialSize() );
				continue;
			}

			// Otherwise serialize data
			property->SerializeProperty( InArchive, InData );

			// Make sure we serialized the right amount of stuff
			uint32		sizeSerialized = InArchive.Tell() - currentOffset;
			if ( sizeSerialized != propertyTag.GetSerialSize() )
			{
				Sys_Error( TEXT( "%s %s: Serial size mismatch: Got %i, Expected %i" ), propertyTag.GetClassName().ToString().c_str(), propertyTag.GetPropertyName().ToString().c_str(), sizeSerialized, propertyTag.GetSerialSize() );
			}
		}
	}
}

/*
==================
CStruct::SerializeBinaryProperties
==================
*/
void CStruct::SerializeBinaryProperties( class CArchive& InArchive, byte* InData )
{
	for ( TFieldIterator<CProperty> it( this ); it; ++it )
	{
		it->SerializeProperty( InArchive, InData );
	}
}

#if WITH_EDITOR
/*
==================
CStruct::ExportProperties
==================
*/
void CStruct::ExportProperties( std::wstring& OutValueString, byte* InData, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	// Open struct scope
	OutValueString = TEXT( "{ " );

	// Export properties
	bool	bMoreThanOneProperty = false;
	for ( TFieldIterator<CProperty> it( this ); it; ++it )
	{
		CProperty*		property = *it;
		std::wstring	valueString;
		property->ExportProperty( valueString, InData, InExportRootScope, InPortFlags );
		if ( !valueString.empty() )
		{
			if ( bMoreThanOneProperty )
			{
				OutValueString += TEXT( ", " );
			}
			OutValueString += L_Sprintf( TEXT( "\"%s\": %s" ), property->GetName().c_str(), valueString.c_str() );
			bMoreThanOneProperty = true;
		}
	}

	// Close struct scope
	OutValueString += TEXT( " }" );
}

/*
==================
CStruct::ImportProperty
==================
*/
void CStruct::ImportProperty( const CJsonValue* InJsonValue, byte* InData, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	// Get JSON struct object
	Assert( InJsonValue );
	const CJsonObject*		jsonStructObject = InJsonValue->GetObject();
	if ( !jsonStructObject )
	{
		Warnf( TEXT( "Invalid JSON data in struct '%s'\n" ), GetName().c_str() );
		return;
	}

	// Import properties
	for ( TFieldIterator<CProperty> it( this ); it; ++it )
	{
		CProperty*			property = *it;
		const CJsonValue*	jsonStructProperty = jsonStructObject->GetValue( property->GetName().c_str() );
		if ( !jsonStructProperty )
		{
			Warnf( TEXT( "Property '%s' not found in the JSON data of '%s'\n" ), property->GetName().c_str(), GetName().c_str() );
			continue;
		}

		property->ImportProperty( jsonStructProperty, ( byte* )InData, InImportRootScope, InPortFlags );
	}
}
#endif // WITH_EDITOR