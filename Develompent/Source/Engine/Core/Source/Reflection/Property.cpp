#include "Logger/LoggerMacros.h"
#include "Reflection/Class.h"
#include "Reflection/Property.h"
#include "Reflection/Object.h"

IMPLEMENT_CLASS( CProperty )
IMPLEMENT_CLASS( CByteProperty )
IMPLEMENT_CLASS( CIntProperty )
IMPLEMENT_CLASS( CBoolProperty )
IMPLEMENT_CLASS( CFloatProperty )
IMPLEMENT_CLASS( CColorProperty )
IMPLEMENT_CLASS( CObjectProperty )
IMPLEMENT_CLASS( CVectorProperty )
IMPLEMENT_CLASS( CRotatorProperty )
IMPLEMENT_CLASS( CAssetProperty )
IMPLEMENT_CLASS( CArrayProperty )
IMPLEMENT_CLASS( CStructProperty )
IMPLEMENT_CLASS( CStringProperty )

IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CIntProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CBoolProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFloatProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CColorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CVectorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CRotatorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CAssetProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CStringProperty )

/*
==================
CProperty::CProperty
==================
*/
CProperty::CProperty()
	: offset( 0 )
	, flags( CPF_None )
{}

/*
==================
CProperty::CProperty
==================
*/
CProperty::CProperty( ECppProperty, uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize /*= 1*/ )
	: category( InCategory )
	, description( InDescription )
	, offset( InOffset )
	, flags( InFlags )
	, arraySize( InArraySize )
{
	// Properties created in C++ should always be marked OBJECT_Transient so that when the package containing
	// this property is saved, it doesn't try to save this CProperty into the ExportMap
	AddObjectFlag( OBJECT_Transient | OBJECT_Native );
	GetOuterCField()->AddProperty( this );
	Assert( GetOuterCField()->HasAllObjectFlags( OBJECT_Transient | OBJECT_Native ) );
}

/*
==================
CProperty::CProperty
==================
*/
CProperty::CProperty( uint32 InOffset, const CName& InCategory, const std::wstring& InDescription, uint32 InFlags, uint32 InArraySize /* = 1 */ )
	: category( InCategory )
	, description( InDescription )
	, offset( InOffset )
	, flags( InFlags )
	, arraySize( InArraySize )
{
	GetOuterCField()->AddProperty( this );
}

/*
==================
CProperty::IsContainsObjectReference
==================
*/
bool CProperty::IsContainsObjectReference() const
{
	return false;
}

/*
==================
CProperty::EmitReferenceInfo
==================
*/
void CProperty::EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const
{}

/*
==================
CProperty::Serialize
==================
*/
void CProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << category;
	InArchive << description;
	InArchive << offset;
	InArchive << flags;
	InArchive << arraySize;
}

/*
==================
CProperty::ShouldSerializeValue
==================
*/
bool CProperty::ShouldSerializeValue( CArchive& InArchive ) const
{
	// Archive can override CArchive::ShouldSkipProperty for filter any properties
	if ( InArchive.ShouldSkipProperty( this ) )
	{
		return false;
	}

	// We shouldn't serialize value if the property hasn't CPF_SaveGame flag, but this archive has
	if ( !HasAnyFlags( CPF_SaveGame ) && InArchive.IsSaveGame() )
	{
		return false;
	}

	// We must filter properties which have flags either CPF_Transient, or CPF_Deprecated, or CPF_EditorOnly
	const uint64		skipFlags = CPF_Transient | CPF_Deprecated | CPF_EditorOnly;
	if ( !HasAnyFlags( skipFlags ) )
	{
		return true;
	}

	bool	bSkip = HasAnyFlags( CPF_Transient ) ||
					( HasAnyFlags( CPF_Deprecated ) && ( InArchive.IsSaving() || InArchive.WantBinaryPropertySerialization() ) ) ||
					( HasAnyFlags( CPF_EditorOnly ) && InArchive.IsFilterEditorOnly() );
	return !bSkip;
}


/*
==================
CByteProperty::StaticInitializeClass
==================
*/
void CByteProperty::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CByteProperty, cenum ) );
}

/*
==================
CByteProperty::GetPropertyValue
==================
*/
uint32 CByteProperty::GetElementSize() const
{
	return sizeof( byte );
}

/*
==================
CByteProperty::GetPropertyValue
==================
*/
uint32 CByteProperty::GetMinAlignment() const
{
	return alignof( byte );
}

/*
==================
CByteProperty::GetPropertyValue
==================
*/
bool CByteProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.byteValue = *( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CByteProperty::SetPropertyValue
==================
*/
bool CByteProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( InObjectAddress + offset ) = InPropertyValue.byteValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CByteProperty::Serialize
==================
*/
void CByteProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << cenum;
}

/*
==================
CByteProperty::SerializeValue
==================
*/
void CByteProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CIntProperty::GetPropertyValue
==================
*/
uint32 CIntProperty::GetElementSize() const
{
	return sizeof( int32 );
}

/*
==================
CIntProperty::GetPropertyValue
==================
*/
uint32 CIntProperty::GetMinAlignment() const
{
	return alignof( int32 );
}

/*
==================
CIntProperty::GetPropertyValue
==================
*/
bool CIntProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.intValue = *( int32* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CIntProperty::SetPropertyValue
==================
*/
bool CIntProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( int32* )( InObjectAddress + offset ) = InPropertyValue.intValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CIntProperty::SerializeValue
==================
*/
void CIntProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( int32* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CFloatProperty::GetPropertyValue
==================
*/
uint32 CFloatProperty::GetElementSize() const
{
	return sizeof( float );
}

/*
==================
CFloatProperty::GetPropertyValue
==================
*/
uint32 CFloatProperty::GetMinAlignment() const
{
	return alignof( float );
}

/*
==================
CFloatProperty::GetPropertyValue
==================
*/
bool CFloatProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.floatValue = *( float* )( InObjectAddress + offset );	
		bResult = true;
	}

	return bResult;
}

/*
==================
CFloatProperty::SetPropertyValue
==================
*/
bool CFloatProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( float* )( InObjectAddress + offset ) = InPropertyValue.floatValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CFloatProperty::SerializeValue
==================
*/
void CFloatProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( float* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CBoolProperty::GetPropertyValue
==================
*/
uint32 CBoolProperty::GetElementSize() const
{
	return sizeof( bool );
}

/*
==================
CBoolProperty::GetPropertyValue
==================
*/
uint32 CBoolProperty::GetMinAlignment() const
{
	return alignof( bool );
}

/*
==================
CBoolProperty::GetPropertyValue
==================
*/
bool CBoolProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.boolValue = *( bool* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CFloatProperty::SetPropertyValue
==================
*/
bool CBoolProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( bool* )( InObjectAddress + offset ) = InPropertyValue.boolValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CBoolProperty::SerializeValue
==================
*/
void CBoolProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( bool* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CColorProperty::GetPropertyValue
==================
*/
uint32 CColorProperty::GetElementSize() const
{
	return sizeof( CColor );
}

/*
==================
CColorProperty::GetPropertyValue
==================
*/
uint32 CColorProperty::GetMinAlignment() const
{
	return alignof( CColor );
}

/*
==================
CColorProperty::GetPropertyValue
==================
*/
bool CColorProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.colorValue = *( CColor* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CColorProperty::SetPropertyValue
==================
*/
bool CColorProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( CColor* )( InObjectAddress + offset ) = InPropertyValue.colorValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CColorProperty::SerializeValue
==================
*/
void CColorProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( CColor* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CObjectProperty::StaticInitializeClass
==================
*/
void CObjectProperty::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CObjectProperty, propertyClass ) );
}

/*
==================
CObjectProperty::GetPropertyValue
==================
*/
uint32 CObjectProperty::GetElementSize() const
{
	return sizeof( void* );
}

/*
==================
CObjectProperty::GetPropertyValue
==================
*/
uint32 CObjectProperty::GetMinAlignment() const
{
	return alignof( void* );
}

/*
==================
CObjectProperty::GetPropertyValue
==================
*/
bool CObjectProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.objectValue = *( CObject** )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CObjectProperty::SetPropertyValue
==================
*/
bool CObjectProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( CObject** )( InObjectAddress + offset ) = InPropertyValue.objectValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CObjectProperty::IsContainsObjectReference
==================
*/
bool CObjectProperty::IsContainsObjectReference() const
{
	return true;
}

/*
==================
CObjectProperty::EmitReferenceInfo
==================
*/
void CObjectProperty::EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const
{
	GCReferenceFixedArrayTokenHelper fixedArrayHelper( InReferenceTokenStream, InBaseOffset + offset, arraySize, sizeof( CObject* ) );
	InReferenceTokenStream->EmitReferenceInfo( GCReferenceInfo( GCRT_Object, InBaseOffset + offset ) );
}

/*
==================
CObjectProperty::Serialize
==================
*/
void CObjectProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << propertyClass;
}

/*
==================
CObjectProperty::SerializeValue
==================
*/
void CObjectProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( CObject** )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CVectorProperty::GetPropertyValue
==================
*/
uint32 CVectorProperty::GetElementSize() const
{
	return sizeof( Vector );
}

/*
==================
CVectorProperty::GetPropertyValue
==================
*/
uint32 CVectorProperty::GetMinAlignment() const
{
	return alignof( Vector );
}

/*
==================
CVectorProperty::GetPropertyValue
==================
*/
bool CVectorProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.vectorValue = *( Vector* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CVectorProperty::SetPropertyValue
==================
*/
bool CVectorProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( Vector* )( InObjectAddress + offset ) = InPropertyValue.vectorValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CVectorProperty::Serialize
==================
*/
void CVectorProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << defaultComponentValue;
}

/*
==================
CVectorProperty::SerializeValue
==================
*/
void CVectorProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( Vector* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CRotatorProperty::GetPropertyValue
==================
*/
uint32 CRotatorProperty::GetElementSize() const
{
	return sizeof( CRotator );
}

/*
==================
CRotatorProperty::GetPropertyValue
==================
*/
uint32 CRotatorProperty::GetMinAlignment() const
{
	return alignof( CRotator );
}

/*
==================
CRotatorProperty::GetPropertyValue
==================
*/
bool CRotatorProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.rotatorValue = *( CRotator* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CRotatorProperty::SetPropertyValue
==================
*/
bool CRotatorProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( CRotator* )( InObjectAddress + offset ) = InPropertyValue.rotatorValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CRotatorProperty::SerializeValue
==================
*/
void CRotatorProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( CRotator* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CAssetProperty::GetPropertyValue
==================
*/
uint32 CAssetProperty::GetElementSize() const
{
	return sizeof( TAssetHandle<CAsset> );
}

/*
==================
CAssetProperty::GetPropertyValue
==================
*/
uint32 CAssetProperty::GetMinAlignment() const
{
	return alignof( TAssetHandle<CAsset> );
}

/*
==================
CAssetProperty::GetPropertyValue
==================
*/
bool CAssetProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.assetValue = *( TAssetHandle<CAsset>* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CAssetProperty::SetPropertyValue
==================
*/
bool CAssetProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( TAssetHandle<CAsset>* )( InObjectAddress + offset ) = InPropertyValue.assetValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CAssetProperty::Serialize
==================
*/
void CAssetProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << assetType;
}

/*
==================
CAssetProperty::SerializeValue
==================
*/
void CAssetProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( TAssetHandle<CAsset>* )( InData + InArrayIdx * GetElementSize() );
}


/*
==================
CArrayProperty::StaticInitializeClass
==================
*/
void CArrayProperty::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CArrayProperty, innerProperty ) );
}

/*
==================
CArrayProperty::AddProperty
==================
*/
void CArrayProperty::AddProperty( class CProperty* InProperty )
{
	innerProperty = InProperty;
}

/*
==================
CArrayProperty::GetPropertyValue
==================
*/
uint32 CArrayProperty::GetElementSize() const
{
	return sizeof( std::vector<byte> );
}

/*
==================
CArrayProperty::GetPropertyValue
==================
*/
uint32 CArrayProperty::GetMinAlignment() const
{
	return alignof( std::vector<byte> );
}

/*
==================
CArrayProperty::GetPropertyValue
==================
*/
bool CArrayProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.arrayValue = ( std::vector<byte>* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CArrayProperty::SetPropertyValue
==================
*/
bool CArrayProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		std::vector<byte>*		dstArray = ( std::vector<byte>* )( InObjectAddress + offset );
		dstArray->resize( InPropertyValue.arrayValue->size() );
		Memory::Memcpy( dstArray->data(), InPropertyValue.arrayValue->data(), dstArray->size() );
		bResult = true;
	}
	return bResult;
}

/*
==================
CArrayProperty::IsContainsObjectReference
==================
*/
bool CArrayProperty::IsContainsObjectReference() const
{
	Assert( innerProperty );
	return innerProperty->IsContainsObjectReference();
}

/*
==================
CArrayProperty::EmitReferenceInfo
==================
*/
void CArrayProperty::EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const
{
	if ( innerProperty->IsContainsObjectReference() )
	{
		if ( IsA<CStructProperty>( innerProperty ) )
		{
			InReferenceTokenStream->EmitReferenceInfo( GCReferenceInfo( GCRT_ArrayStruct, InBaseOffset + offset ) );
			InReferenceTokenStream->EmitStride( innerProperty->GetElementSize() );
			const uint32	skipIndexIndex = InReferenceTokenStream->EmitSkipIndexPlaceholder();
			innerProperty->EmitReferenceInfo( InReferenceTokenStream, 0 );
			const uint32	skipIndex = InReferenceTokenStream->EmitReturn();
			InReferenceTokenStream->UpdateSkipIndexPlaceholder( skipIndexIndex, skipIndex );
		}
		else if ( IsA<CObjectProperty>( innerProperty ) )
		{
			InReferenceTokenStream->EmitReferenceInfo( GCReferenceInfo( GCRT_ArrayObject, InBaseOffset + offset ) );
		}
		else
		{
			Sys_Error( TEXT( "Encountered unknown property containing object or name reference: %s in %s" ), innerProperty->GetName().c_str(), GetName().c_str() );
		}
	}
}

/*
==================
CArrayProperty::Serialize
==================
*/
void CArrayProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << innerProperty;
}

/*
==================
CArrayProperty::SerializeValue
==================
*/
void CArrayProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	std::vector<byte>*	arrayItems			= ( std::vector<byte>* )( InData + InArrayIdx * GetElementSize() );
	uint32				innerPropertySize	= innerProperty->GetElementSize();
	uint32				numItems			= arrayItems->size() / innerPropertySize;
	InArchive << numItems;

	// Serialize array size
	if ( InArchive.IsLoading() )
	{
		arrayItems->resize( numItems * innerPropertySize );
	}
	
	// Serialize items
	byte*	data = arrayItems->data();
	for ( uint32 index = 0; index < numItems; ++index )
	{
		innerProperty->SerializeValue( InArchive, data, index );
	}
}


/*
==================
CStructProperty::StaticInitializeClass
==================
*/
void CStructProperty::StaticInitializeClass()
{
	CClass*		theClass = StaticClass();
	theClass->EmitObjectReference( STRUCT_OFFSET( CStructProperty, propertyStruct ) );
}

/*
==================
CStructProperty::GetPropertyValue
==================
*/
uint32 CStructProperty::GetElementSize() const
{
	return propertyStruct->GetPropertiesSize();
}

/*
==================
CStructProperty::GetPropertyValue
==================
*/
uint32 CStructProperty::GetMinAlignment() const
{
	return propertyStruct->GetMinAlignment();
}

/*
==================
CStructProperty::GetPropertyValue
==================
*/
bool CStructProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	return false;
}

/*
==================
CStructProperty::SetPropertyValue
==================
*/
bool CStructProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	return false;
}

/*
==================
CStructProperty::IsContainsObjectReference
==================
*/
bool CStructProperty::IsContainsObjectReference() const
{
	// Prevent recursion in the case of structs containing dynamic arrays of themselves
	static std::list<const CStructProperty*>	encounteredStructProps;
	if ( std::find( encounteredStructProps.begin(), encounteredStructProps.end(), this ) != encounteredStructProps.end() )
	{
		return false;
	}
	else
	{
		Assert( propertyStruct );
		encounteredStructProps.push_back( this );

		std::vector<CProperty*>		properties;
		propertyStruct->GetProperties( properties );
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			CProperty*	property = properties[index];
			if ( property->IsContainsObjectReference() )
			{
				encounteredStructProps.erase( std::find( encounteredStructProps.begin(), encounteredStructProps.end(), this ) );
				return true;
			}
		}

		encounteredStructProps.erase( std::find( encounteredStructProps.begin(), encounteredStructProps.end(), this ) );
		return false;
	}
}

/*
==================
CStructProperty::EmitReferenceInfo
==================
*/
void CStructProperty::EmitReferenceInfo( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InBaseOffset ) const
{
	Assert( propertyStruct );
	if ( IsContainsObjectReference() )
	{
		GCReferenceFixedArrayTokenHelper	fixedArrayHelper( InReferenceTokenStream, InBaseOffset + offset, arraySize, GetElementSize() );
		std::vector<CProperty*>				properties;
		
		propertyStruct->GetProperties( properties );
		for ( uint32 index = 0, count = properties.size(); index < count; ++index )
		{
			CProperty*	property = properties[index];
			property->EmitReferenceInfo( InReferenceTokenStream, InBaseOffset + offset );
		}
	}
}

/*
==================
CStructProperty::Serialize
==================
*/
void CStructProperty::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << propertyStruct;
}

/*
==================
CStructProperty::SerializeValue
==================
*/
void CStructProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	byte*	data = InData + InArrayIdx * GetElementSize();
	propertyStruct->SerializeProperties( InArchive, data );
}


/*
==================
CStringProperty::GetPropertyValue
==================
*/
uint32 CStringProperty::GetElementSize() const
{
	return sizeof( std::wstring );
}

/*
==================
CStringProperty::GetPropertyValue
==================
*/
uint32 CStringProperty::GetMinAlignment() const
{
	return alignof( std::wstring );
}

/*
==================
CStringProperty::GetPropertyValue
==================
*/
bool CStringProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.stringValue = ( std::wstring* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CStringProperty::SetPropertyValue
==================
*/
bool CStringProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( !( flags & CPF_Const ) && InObjectAddress )
	{
		*( std::wstring* )( InObjectAddress + offset ) = *InPropertyValue.stringValue;
		bResult = true;
	}
	return bResult;
}

/*
==================
CStringProperty::SerializeValue
==================
*/
void CStringProperty::SerializeValue( class CArchive& InArchive, byte* InData, uint32 InArrayIdx ) const
{
	InArchive << *( std::wstring* )( InData + InArrayIdx * GetElementSize() );
}