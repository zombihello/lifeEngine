#include "Misc/Class.h"
#include "Misc/Property.h"
#include "Misc/Object.h"

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

IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CByteProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CIntProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CBoolProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFloatProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CColorProperty )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObjectProperty )
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.byteValue = **( byte** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.byteValue = *( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( byte** )( InObjectAddress + offset ) = InPropertyValue.byteValue;
		}
		else
		{
			*( InObjectAddress + offset ) = InPropertyValue.byteValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.intValue = **( int32** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.intValue = *( int32* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( int32** )( InObjectAddress + offset ) = InPropertyValue.intValue;
		}
		else
		{
			*( int32* )( InObjectAddress + offset ) = InPropertyValue.intValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.floatValue = **( float** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.floatValue = *( float* )( InObjectAddress + offset );
		}		
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
		if ( flags & CPF_Pointer )
		{
			**( float** )( InObjectAddress + offset ) = InPropertyValue.floatValue;
		}
		else
		{
			*( float* )( InObjectAddress + offset ) = InPropertyValue.floatValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.boolValue = **( bool** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.boolValue = *( bool* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( bool** )( InObjectAddress + offset ) = InPropertyValue.boolValue;
		}
		else
		{
			*( bool* )( InObjectAddress + offset ) = InPropertyValue.boolValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.colorValue = **( CColor** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.colorValue = *( CColor* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( CColor** )( InObjectAddress + offset ) = InPropertyValue.colorValue;
		}
		else
		{
			*( CColor* )( InObjectAddress + offset ) = InPropertyValue.colorValue;
		}
		bResult = true;
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.objectValue = *( CObject** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.objectValue = ( CObject* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			*( CObject** )( InObjectAddress + offset ) = InPropertyValue.objectValue;
		}
		else
		{
			memcpy( InObjectAddress + offset, InPropertyValue.objectValue, sizeof( CObject* ) );
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.vectorValue = **( Vector** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.vectorValue = *( Vector* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( Vector** )( InObjectAddress + offset ) = InPropertyValue.vectorValue;
		}
		else
		{
			*( Vector* )( InObjectAddress + offset ) = InPropertyValue.vectorValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.rotatorValue = **( CRotator** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.rotatorValue = *( CRotator* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( CRotator** )( InObjectAddress + offset ) = InPropertyValue.rotatorValue;
		}
		else
		{
			*( CRotator* )( InObjectAddress + offset ) = InPropertyValue.rotatorValue;
		}
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
		if ( flags & CPF_Pointer )
		{
			OutPropertyValue.assetValue = **( TAssetHandle<CAsset>** )( InObjectAddress + offset );
		}
		else
		{
			OutPropertyValue.assetValue = *( TAssetHandle<CAsset>* )( InObjectAddress + offset );
		}
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
		if ( flags & CPF_Pointer )
		{
			**( TAssetHandle<CAsset>** )( InObjectAddress + offset ) = InPropertyValue.assetValue;
		}
		else
		{
			*( TAssetHandle<CAsset>* )( InObjectAddress + offset ) = InPropertyValue.assetValue;
		}
		bResult = true;
	}
	return bResult;
}