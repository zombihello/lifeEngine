#include "Misc/Class.h"
#include "Misc/Property.h"
#include "Components/ActorComponent.h"

IMPLEMENT_CLASS( CProperty )
IMPLEMENT_CLASS( CByteProperty )
IMPLEMENT_CLASS( CIntProperty )
IMPLEMENT_CLASS( CBoolProperty )
IMPLEMENT_CLASS( CFloatProperty )
IMPLEMENT_CLASS( CColorProperty )
IMPLEMENT_CLASS( CComponentProperty )
IMPLEMENT_CLASS( CVectorProperty )
IMPLEMENT_CLASS( CRotatorProperty )
IMPLEMENT_CLASS( CAssetProperty )

IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CByteProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CIntProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CBoolProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFloatProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CColorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CComponentProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CVectorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CRotatorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CAssetProperty )

/*
==================
CProperty::CProperty
==================
*/
CProperty::CProperty()
	: offset( 0 )
	, size( 0 )
	, flags( CPF_None )
{}

/*
==================
CProperty::CProperty
==================
*/
CProperty::CProperty( CClass* InClass, const CName& InName, const CName& InCategory, const std::wstring& InDescription, uint32 InOffset, uint32 InSize, uint32 InFlags )
	: category( InCategory )
	, description( InDescription )
	, offset( InOffset )
	, size( InSize )
	, flags( InFlags )
{
	SetCName( InName );
	InClass->AddProperty( this );
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
	if ( InObjectAddress )
	{
		*( InObjectAddress + offset ) = InPropertyValue.byteValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( int32* )( InObjectAddress + offset ) = InPropertyValue.intValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( float* )( InObjectAddress + offset ) = InPropertyValue.floatValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( bool* )( InObjectAddress + offset ) = InPropertyValue.boolValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( CColor* )( InObjectAddress + offset ) = InPropertyValue.colorValue;
		bResult = true;
	}
	return bResult;
}


/*
==================
CComponentProperty::GetPropertyValue
==================
*/
bool CComponentProperty::GetPropertyValue( byte* InObjectAddress, UPropertyValue& OutPropertyValue ) const
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		OutPropertyValue.componentValue = *( TRefCountPtr<CActorComponent>* )( InObjectAddress + offset );
		bResult = true;
	}

	return bResult;
}

/*
==================
CComponentProperty::SetPropertyValue
==================
*/
bool CComponentProperty::SetPropertyValue( byte* InObjectAddress, const UPropertyValue& InPropertyValue )
{
	bool	bResult = false;
	if ( InObjectAddress )
	{
		*( TRefCountPtr<CActorComponent>* )( InObjectAddress + offset ) = InPropertyValue.componentValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( Vector* )( InObjectAddress + offset ) = InPropertyValue.vectorValue;
		bResult = true;
	}
	return bResult;
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
	if ( InObjectAddress )
	{
		*( CRotator* )( InObjectAddress + offset ) = InPropertyValue.rotatorValue;
		bResult = true;
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
	if ( InObjectAddress )
	{
		*( TAssetHandle<CAsset>* )( InObjectAddress + offset ) = InPropertyValue.assetValue;
		bResult = true;
	}
	return bResult;
}