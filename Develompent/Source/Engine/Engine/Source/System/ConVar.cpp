#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "System/ConVar.h"
#include "System/ConsoleSystem.h"

/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const std::wstring& InName, const std::wstring& InDefaultValue, EConVarType InType, const std::wstring& InHelpText, bool InHasMin, float InMin, bool InHasMax, float InMax, bool InIsReadOnly /* = false */ )
	: bHasMin( InHasMin )
	, bHasMax( InHasMax )
	, bReadOnly( InIsReadOnly )
	, name( InName )
	, helpText( InHelpText )
	, defaultValue( InDefaultValue )
	, type( CVT_None )
	, data( nullptr )
	, minVar( InMin )
	, maxVar( InMax )
{
	GetGlobalConVars().push_back( this );
	SetValue( InDefaultValue, InType );
}

/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const std::wstring& InName, const std::wstring& InDefaultValue, EConVarType InType, const std::wstring& InHelpText, bool InIsReadOnly /* = false */ )
	: bHasMin( false )
	, bHasMax( false )
	, bReadOnly( InIsReadOnly )
	, name( InName )
	, helpText( InHelpText )
	, defaultValue( InDefaultValue )
	, type( CVT_None )
	, data( nullptr )
	, minVar( 0.f )
	, maxVar( 0.f )
{
	GetGlobalConVars().push_back( this );
	SetValue( InDefaultValue, InType );
}

/*
==================
CConVar::~CConVar
==================
*/
CConVar::~CConVar()
{
	std::vector<CConVar*>&	vars = GetGlobalConVars();
	for ( uint32 index = 0, count = vars.size(); index < count; ++index )
	{
		if ( vars[index] == this )
		{
			vars.erase( vars.begin() + index );
			break;
		}
	}

	DeleteValue();
}

/*
==================
CConVar::SetValue
==================
*/
void CConVar::SetValue( const std::wstring& InValue, EConVarType InVarType )
{
	if ( bReadOnly )
	{
		return;
	}

	switch ( InVarType )
	{
	case CVT_Int:
		SetValueInt( stoi( InValue ) );
		break;

	case CVT_Float:
		SetValueFloat( stof( InValue ) );
		break;

	case CVT_Bool:
	{
		std::wstring	valueLower = L_Strlwr( InValue );
		SetValueBool( valueLower == TEXT( "true" ) || stoi( valueLower ) ? true : false );
		break;
	}

	case CVT_String:
		SetValueString( InValue );
		break;

	default:
		AssertMsg( false, TEXT( "Unknown ConVar type 0x%X" ), InVarType );
	}
}

/*
==================
CConVar::SetValueInt
==================
*/
void CConVar::SetValueInt( int32 InValue )
{
	if ( bReadOnly )
	{
		return;
	}

	if ( type != CVT_None && type != CVT_Int )
	{
		DeleteValue();
	}

	if ( type == CVT_None )
	{
		data = new int32;
	}

	int32*	intData = ( int32* )data;
	if ( bHasMin && InValue < minVar )
	{
		*intData = minVar;
	}
	else if ( bHasMax && InValue > maxVar )
	{
		*intData = maxVar;
	}
	else
	{
		*intData = InValue;
	}

	type = CVT_Int;
	onChangeVar.Broadcast( this );
}

/*
==================
CConVar::SetValueFloat
==================
*/
void CConVar::SetValueFloat( float InValue )
{
	if ( bReadOnly )
	{
		return;
	}

	if ( type != CVT_None && type != CVT_Float )
	{
		DeleteValue();
	}

	if ( type == CVT_None )
	{
		data = new float;
	}

	float*		floatData = ( float* )data;
	if ( bHasMin && InValue < minVar )
	{
		*floatData = minVar;
	}
	else if ( bHasMax && InValue > maxVar )
	{
		*floatData = maxVar;
	}
	else
	{
		*floatData = InValue;
	}

	type = CVT_Float;
	onChangeVar.Broadcast( this );
}

/*
==================
CConVar::SetValueBool
==================
*/
void CConVar::SetValueBool( bool InValue )
{
	if ( bReadOnly )
	{
		return;
	}

	if ( type != CVT_None && type != CVT_Bool )
	{
		DeleteValue();
	}

	if ( type == CVT_None )
	{
		data = new bool;
	}

	bool*	boolData = ( bool* )data;
	SetMin( true, 0.f );
	SetMax( true, 1.f );

	if ( bHasMin && ( float )InValue < minVar )
	{
		*boolData = minVar;
	}
	else if ( bHasMax && ( float )InValue > maxVar )
	{
		*boolData = maxVar;
	}
	else
	{
		*boolData = InValue;
	}

	type = CVT_Bool;
	onChangeVar.Broadcast( this );
}

/*
==================
CConVar::SetValueString
==================
*/
void CConVar::SetValueString( const std::wstring& InValue )
{
	if ( bReadOnly )
	{
		return;
	}

	if ( type != CVT_None && type != CVT_String )
	{
		DeleteValue();
	}

	if ( type == CVT_None )
	{
		data = new std::wstring();
	}

	std::wstring*	stringData = ( std::wstring* )data;
	if ( bHasMin && InValue.size() < minVar )
	{
		*stringData = InValue;
		stringData->append( minVar - InValue.size(), TEXT( ' ' ) );
	}
	else if ( bHasMax && InValue.size() > maxVar )
	{
		*stringData = InValue.substr( 0, maxVar );
	}
	else
	{
		*stringData = InValue;
	}

	type = CVT_String;
	onChangeVar.Broadcast( this );
}

/*
==================
CConVar::DeleteValue
==================
*/
void CConVar::DeleteValue()
{
	if ( type == CVT_None )
	{
		return;
	}

	switch ( type )
	{
	case CVT_Int:       delete static_cast<int32*>( data );				break;
	case CVT_Bool:      delete static_cast<bool*>( data );				break;
	case CVT_Float:     delete static_cast<float*>( data );				break;
	case CVT_String:    delete static_cast<std::wstring*>( data );		break;
	}

	type = CVT_None;
}