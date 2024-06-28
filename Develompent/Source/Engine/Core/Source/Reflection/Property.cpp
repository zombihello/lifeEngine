#include "Logger/LoggerMacros.h"
#include "Reflection/Class.h"
#include "Reflection/Property.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/FieldIterator.h"

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
{}

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

#if WITH_EDITOR
/*
==================
CProperty::ShouldPort
==================
*/
bool CProperty::ShouldPort( uint32 InPortFlags /* = PPF_None */ ) const
{
	// Don't export/import deprecated properties if we needn't it
	if ( HasAnyFlags( CPF_Deprecated ) && !( InPortFlags & PPF_UseDeprecatedProperties ) )
	{
		return false;
	}

	// Don't export/import transient properties if the user doesn't want it
	if ( HasAnyFlags( CPF_Transient ) && !( InPortFlags & PPF_IncludeTransient ) )
	{
		return false;
	}

	// Hide non-Edit properties when we're exporting for the property window
	if ( ( InPortFlags & PPF_PropertyWindow ) && !HasAnyFlags( CPF_Edit ) )
	{
		return false;
	}

	return true;
}

/*
==================
CProperty::ExportProperty
==================
*/
void CProperty::ExportProperty( std::wstring& OutValueString, byte* InObjectAddress, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	OutValueString.clear();
	if ( ShouldPort( InPortFlags ) )
	{
		// Get the begin of property data and open array scope if arraySize > 1
		byte*	data = InObjectAddress + offset;
		if ( arraySize > 1 )
		{
			OutValueString = TEXT( "[ " );
		}

		// Export values
		for ( uint32 index = 0; index < arraySize; ++index )
		{
			std::wstring	valueString;
			ExportValue( valueString, data, index, InExportRootScope, InPortFlags );
			OutValueString += valueString;
			if ( arraySize > 1 && index + 1 < arraySize )
			{
				OutValueString += TEXT( ", " );
			}
		}

		// Close array scope if it need
		if ( arraySize > 1 )
		{
			OutValueString += TEXT( " ]" );
		}
	}
}

/*
==================
CProperty::ImportProperty
==================
*/
void CProperty::ImportProperty( const CJsonValue* InJsonValue, byte* InObjectAddress, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	if ( ShouldPort( InPortFlags ) )
	{
		Assert( InJsonValue );
		const std::vector<CJsonValue>*	values = InJsonValue->GetArray();
		bool							bIsArray = arraySize > 1;
		if ( bIsArray && !values )
		{
			Warnf( TEXT( "Invalid JSON data in property '%s'\n" ), GetName().c_str() );
			return;
		}

		// Get the begin of property data
		byte*		data = InObjectAddress + offset;

		// Import values
		if ( bIsArray )
		{
			for ( uint32 index = 0, count = values->size(); index < count && index < arraySize; ++index )
			{
				const CJsonValue&	jsonValue = values->at( index );
				ImportValue( &jsonValue, data, index, InImportRootScope, InPortFlags );
			}
		}
		else
		{
			ImportValue( InJsonValue, data, 0, InImportRootScope, InPortFlags );
		}
	}
}
#endif // WITH_EDITOR

/*
==================
CProperty::IsIdentical
==================
*/
bool CProperty::IsIdentical( CProperty* InProperty ) const
{
	// Check on identical class
	Assert( InProperty );
	if ( GetClass() != InProperty->GetClass() )
	{
		return false;
	}

	// Check on identical array size
	if ( arraySize != InProperty->GetArraySize() )
	{
		return false;
	}

	// Check on identical flags
	if ( flags != InProperty->GetFlags() )
	{
		return false;
	}

	// All ok
	return true;
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
CByteProperty::IsIdentical
==================
*/
bool CByteProperty::IsIdentical( CProperty* InProperty ) const
{
	bool	bResult = Super::IsIdentical( InProperty );
	if ( cenum != ( ( CByteProperty* )InProperty )->cenum )
	{
		bResult = false;
	}

	return bResult;
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
CByteProperty::CopyPropertyValue
==================
*/
void CByteProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CByteProperty::ExportValue
==================
*/
void CByteProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	OutValueString = L_Sprintf( TEXT( "%i" ), *( InData + InArrayIdx * GetElementSize() ) );
}

/*
==================
CByteProperty::ImportValue
==================
*/
void CByteProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_Int ) && !InJsonValue->IsA( JVT_Float ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	byte*	value = InData + InArrayIdx * GetElementSize();
	*value = ( byte )InJsonValue->GetNumber();
}
#endif // WITH_EDITOR


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
CIntProperty::CopyPropertyValue
==================
*/
void CIntProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CIntProperty::ExportValue
==================
*/
void CIntProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	OutValueString = L_Sprintf( TEXT( "%i" ), *( int32* )( InData + InArrayIdx * GetElementSize() ) );
}

/*
==================
CIntProperty::ImportValue
==================
*/
void CIntProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_Bool ) &&  !InJsonValue->IsA( JVT_Int ) && !InJsonValue->IsA( JVT_Float ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	int32*	value = ( int32* )( InData + InArrayIdx * GetElementSize() );
	*value	= ( int32 )InJsonValue->GetNumber();
}
#endif // WITH_EDITOR


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
CFloatProperty::CopyPropertyValue
==================
*/
void CFloatProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CFloatProperty::ExportValue
==================
*/
void CFloatProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	OutValueString = L_Sprintf( TEXT( "%f" ), *( float* )( InData + InArrayIdx * GetElementSize() ) );
}

/*
==================
CFloatProperty::ImportValue
==================
*/
void CFloatProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_Bool ) &&  !InJsonValue->IsA( JVT_Int ) && !InJsonValue->IsA( JVT_Float ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	float*	value = ( float* )( InData + InArrayIdx * GetElementSize() );
	*value	= InJsonValue->GetNumber();
}
#endif // WITH_EDITOR


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
CBoolProperty::CopyPropertyValue
==================
*/
void CBoolProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CBoolProperty::ExportValue
==================
*/
void CBoolProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	bool	bValue = *( bool* )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "%s" ), bValue ? TEXT( "true" ) : TEXT( "false" ) );
}

/*
==================
CBoolProperty::ImportValue
==================
*/
void CBoolProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_Bool ) &&  !InJsonValue->IsA( JVT_Int ) && !InJsonValue->IsA( JVT_Float ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	bool*	value = ( bool* )( InData + InArrayIdx * GetElementSize() );
	*value	= ( bool )InJsonValue->GetNumber();
}
#endif // WITH_EDITOR


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
CColorProperty::CopyPropertyValue
==================
*/
void CColorProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CColorProperty::ExportValue
==================
*/
void CColorProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	CColor*		color = ( CColor* )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "{ \"R\": %i, \"G\": %i, \"B\": %i }" ), color->r, color->g, color->b );
}

/*
==================
CColorProperty::ImportValue
==================
*/
void CColorProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	const CJsonObject*		jsonColorObject = InJsonValue->GetObject();
	if ( !jsonColorObject )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	const CJsonValue*	jsonColorR = jsonColorObject->GetValue( TEXT( "R" ) );
	const CJsonValue*	jsonColorG = jsonColorObject->GetValue( TEXT( "G" ) );
	const CJsonValue*	jsonColorB = jsonColorObject->GetValue( TEXT( "B" ) );
	CColor*				color = ( CColor* )( InData + InArrayIdx * GetElementSize() );
	color->r = jsonColorR ? ( uint8 )jsonColorR->GetNumber() : 0;
	color->g = jsonColorG ? ( uint8 )jsonColorG->GetNumber() : 0;
	color->b = jsonColorB ? ( uint8 )jsonColorB->GetNumber() : 0;
}
#endif // WITH_EDITOR


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
CObjectProperty::IsIdentical
==================
*/
bool CObjectProperty::IsIdentical( CProperty* InProperty ) const
{
	bool	bResult = Super::IsIdentical( InProperty );
	if ( propertyClass != ( ( CObjectProperty* )InProperty )->propertyClass )
	{
		bResult = false;
	}

	return bResult;
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
CObjectProperty::CopyPropertyValue
==================
*/
void CObjectProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	// TODO yehor.pohuliaka - Implement construct a new object (if it need)
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CObjectProperty::ExportValue
==================
*/
void CObjectProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	CObject*		object = *( CObject** )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "\"%s\"" ), object->GetPathName( InExportRootScope ).c_str() );
}

/*
==================
CObjectProperty::ImportValue
==================
*/
void CObjectProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_String ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	CObject**		object = ( CObject** )( InData + InArrayIdx * GetElementSize() );
	std::wstring	objectPathName = InJsonValue->GetString();

	// Firstly we try find the object in our import root scope
	*object = FindObject<CObject>( InImportRootScope, objectPathName.c_str() );

	// Otherwise find the object in global scope
	if ( !*object )
	{
		*object = FindObject<CObject>( ANY_PACKAGE, objectPathName.c_str() );
	}
}
#endif // WITH_EDITOR


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
CVectorProperty::CopyPropertyValue
==================
*/
void CVectorProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CVectorProperty::ExportValue
==================
*/
void CVectorProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Vector*		vector = ( Vector* )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "{ \"X\": %f, \"Y\": %f, \"Z\": %f }" ), vector->x, vector->y, vector->z );
}

/*
==================
CVectorProperty::ImportValue
==================
*/
void CVectorProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	const CJsonObject*		jsonVectorObject = InJsonValue->GetObject();
	if ( !jsonVectorObject )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	const CJsonValue*	jsonVectorX = jsonVectorObject->GetValue( TEXT( "X" ) );
	const CJsonValue*	jsonVectorY = jsonVectorObject->GetValue( TEXT( "Y" ) );
	const CJsonValue*	jsonVectorZ = jsonVectorObject->GetValue( TEXT( "Z" ) );
	Vector*				vector = ( Vector* )( InData + InArrayIdx * GetElementSize() );
	vector->x			= jsonVectorX ? jsonVectorX->GetNumber() : 0.f;
	vector->y			= jsonVectorY ? jsonVectorY->GetNumber() : 0.f;
	vector->z			= jsonVectorZ ? jsonVectorZ->GetNumber() : 0.f;
}
#endif // WITH_EDITOR


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
CRotatorProperty::CopyPropertyValue
==================
*/
void CRotatorProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CRotatorProperty::ExportValue
==================
*/
void CRotatorProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	CRotator*		rotator = ( CRotator* )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "{ \"Pitch\": %f, \"Yaw\": %f, \"Roll\": %f }" ), rotator->pitch, rotator->yaw, rotator->roll );
}

/*
==================
CRotatorProperty::ImportValue
==================
*/
void CRotatorProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	const CJsonObject*		jsonRotatorObject = InJsonValue->GetObject();
	if ( !jsonRotatorObject )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	const CJsonValue*	jsonRotatorPitch = jsonRotatorObject->GetValue( TEXT( "Pitch" ) );
	const CJsonValue*	jsonRotatorYaw = jsonRotatorObject->GetValue( TEXT( "Yaw" ) );
	const CJsonValue*	jsonRotatorRoll = jsonRotatorObject->GetValue( TEXT( "Roll" ) );
	CRotator*			rotator = ( CRotator* )( InData + InArrayIdx * GetElementSize() );
	rotator->pitch		= jsonRotatorPitch ? jsonRotatorPitch->GetNumber() : 0.f;
	rotator->yaw		= jsonRotatorYaw ? jsonRotatorYaw->GetNumber() : 0.f;
	rotator->roll		= jsonRotatorRoll ? jsonRotatorRoll->GetNumber() : 0.f;
}
#endif // WITH_EDITOR


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
CAssetProperty::IsIdentical
==================
*/
bool CAssetProperty::IsIdentical( CProperty* InProperty ) const
{
	bool	bResult = Super::IsIdentical( InProperty );
	if ( assetType != ( ( CAssetProperty* )InProperty )->assetType )
	{
		bResult = false;
	}

	return bResult;
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
CAssetProperty::CopyPropertyValue
==================
*/
void CAssetProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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

#if WITH_EDITOR
/*
==================
CAssetProperty::ExportValue
==================
*/
void CAssetProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	TAssetHandle<CAsset>*		asset = ( TAssetHandle<CAsset>* )( InData + InArrayIdx * GetElementSize() );
	std::wstring				assetReference;
	MakeReferenceToAsset( *asset, assetReference );
	OutValueString = L_Sprintf( TEXT( "\"%s\"" ), assetReference.c_str() );
}

/*
==================
CAssetProperty::ImportValue
==================
*/
void CAssetProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_String ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	TAssetHandle<CAsset>*	asset = ( TAssetHandle<CAsset>* )( InData + InArrayIdx * GetElementSize() );
	std::wstring			assetReference = InJsonValue->GetString();
	*asset = g_PackageManager->FindAsset( assetReference );
}
#endif // WITH_EDITOR


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
CArrayProperty::IsIdentical
==================
*/
bool CArrayProperty::IsIdentical( CProperty* InProperty ) const
{
	bool	bResult = Super::IsIdentical( InProperty );
	if ( !innerProperty->IsIdentical( ( ( CArrayProperty* )InProperty )->innerProperty ) )
	{
		bResult = false;
	}

	return bResult;
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
CArrayProperty::CopyPropertyValue
==================
*/
void CArrayProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	// Do nothing if the pointers are the same
	if ( InDestAddress == InSrcAddress )
	{
		return;
	}

	// TODO yehor.pohuliaka - When was implement construct a new object (if it need) here should call innerProperty->CopyPropertyValue
	for ( uint32 index = 0; index < arraySize; ++index )
	{
		std::vector<byte>*		destArray	= ( ( std::vector<byte>* )InDestAddress ) + index;
		std::vector<byte>*		srcArray	= ( ( std::vector<byte>* )InSrcAddress ) + index;
		destArray->resize( srcArray->size() );
		Memory::Memcpy( destArray->data(), srcArray->data(), destArray->size() );
	}
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

#if WITH_EDITOR
/*
==================
CArrayProperty::ExportValue
==================
*/
void CArrayProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	std::vector<byte>*	arrayItems			= ( std::vector<byte>* )( InData + InArrayIdx * GetElementSize() );
	uint32				innerPropertySize	= innerProperty->GetElementSize();
	uint32				numItems			= arrayItems->size() / innerPropertySize;
	
	// Open array scope
	OutValueString = TEXT( "[ " );
	
	// Export array items
	byte*		data = arrayItems->data();
	for ( uint32 index = 0; index < numItems; ++index )
	{
		std::wstring		valueString;
		innerProperty->ExportValue( valueString, data, index, InExportRootScope, InPortFlags );
		OutValueString += valueString;
		if ( index + 1 < numItems )
		{
			OutValueString += TEXT( ", " );
		}
	}

	// Close array scope
	OutValueString += TEXT( " ]" );
}

/*
==================
CArrayProperty::ImportValue
==================
*/
void CArrayProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_Array ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	const std::vector<CJsonValue>*	jsonValues			= InJsonValue->GetArray();
	std::vector<byte>*				arrayItems			= ( std::vector<byte>* )( InData + InArrayIdx * GetElementSize() );
	uint32							innerPropertySize	= innerProperty->GetElementSize();

	// Resize array and import items
	arrayItems->resize( jsonValues->size() * innerPropertySize );
	byte*	data = arrayItems->data();
	for ( uint32 index = 0, count = jsonValues->size(); index < count; ++index )
	{
		const CJsonValue&	jsonValue = jsonValues->at( index );
		innerProperty->ImportValue( &jsonValue, data, index, InImportRootScope, InPortFlags );
	}
}
#endif // WITH_EDITOR


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
CStructProperty::IsIdentical
==================
*/
bool CStructProperty::IsIdentical( CProperty* InProperty ) const
{
	bool	bResult = Super::IsIdentical( InProperty );
	if ( propertyStruct != ( ( CStructProperty* )InProperty )->propertyStruct )
	{
		bResult = false;
	}

	return bResult;
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
CStructProperty::CopyPropertyValue
==================
*/
void CStructProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	// TODO yehor.pohuliaka - When was implement construct a new object (if it need) here should call CopyPropertyValue for each struct's property
	Memory::Memcpy( InDestAddress, InSrcAddress, GetSize() );
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
		for ( TFieldIterator<CProperty> it( propertyStruct ); it; ++it )
		{
			if ( it->IsContainsObjectReference() )
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
		for ( TFieldIterator<CProperty> it( propertyStruct ); it; ++it )
		{
			it->EmitReferenceInfo( InReferenceTokenStream, InBaseOffset + offset );
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

#if WITH_EDITOR
/*
==================
CStructProperty::ExportValue
==================
*/
void CStructProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	OutValueString.clear();
	byte*		data = InData + InArrayIdx * GetElementSize();
	propertyStruct->ExportProperties( OutValueString, data, InExportRootScope, InPortFlags );
}

/*
==================
CStructProperty::ImportValue
==================
*/
void CStructProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	byte*	data = InData + InArrayIdx * GetElementSize();
	propertyStruct->ImportProperty( InJsonValue, data, InImportRootScope, InPortFlags );
}
#endif // WITH_EDITOR


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
CStringProperty::CopyPropertyValue
==================
*/
void CStringProperty::CopyPropertyValue( byte* InDestAddress, byte* InSrcAddress ) const
{
	for ( uint32 index = 0; index < arraySize; ++index )
	{
		( ( std::wstring* )InDestAddress )[index] = ( ( std::wstring* )InSrcAddress)[index];
	}
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

#if WITH_EDITOR
/*
==================
CStringProperty::ExportValue
==================
*/
void CStringProperty::ExportValue( std::wstring& OutValueString, byte* InData, uint32 InArrayIdx, CObject* InExportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	std::wstring*	string = ( std::wstring* )( InData + InArrayIdx * GetElementSize() );
	OutValueString = L_Sprintf( TEXT( "\"%s\"" ), string->c_str() );
}

/*
==================
CStringProperty::ImportValue
==================
*/
void CStringProperty::ImportValue( const CJsonValue* InJsonValue, byte* InData, uint32 InArrayIdx, CObject* InImportRootScope, uint32 InPortFlags /* = PPF_None */ )
{
	Assert( InJsonValue );
	if ( !InJsonValue->IsA( JVT_String ) )
	{
		Warnf( TEXT( "Invalid JSON property data for '%s'\n" ), GetName().c_str() );
		return;
	}

	std::wstring*	string = ( std::wstring* )( InData + InArrayIdx * GetElementSize() );
	*string = InJsonValue->GetString();
}
#endif // WITH_EDITOR