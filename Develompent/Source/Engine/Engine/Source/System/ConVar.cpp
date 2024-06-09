#include "Misc/EngineGlobals.h"
#include "System/ConVar.h"
#include "System/Cvar.h"

/*
==================
CConCmdBase::CConCmdBase
==================
*/
CConCmdBase::CConCmdBase( const tchar* InName, const tchar* InHelpText /* = TEXT( "" ) */, uint32 InFlags /* = FCVAR_None */ )
	: bRegistered( false )
	, flags( InFlags )
	, name( InName )
	, helpText( InHelpText )
	, next( nullptr )
{
	Assert( name );
	g_Cvar.RegisterCommand( this );
}

/*
==================
CConCmdBase::~CConCmdBase
==================
*/
CConCmdBase::~CConCmdBase()
{
	if ( bRegistered )
	{
		g_Cvar.UnregisterCommand( this );
	}
}


/*
==================
CConCmd::CConCmd
==================
*/
CConCmd::CConCmd( const tchar* InName, ConCmdExecFn_t InExecFn, const tchar* InHelpText /* = TEXT( "" ) */, uint32 InFlags /* = FCVAR_None */ )
	: CConCmdBase( InName, InHelpText, InFlags )
	, ExecFn( InExecFn )
{}


/*
==================
CConCmd::IsCommand
==================
*/
bool CConCmd::IsCommand() const
{
	return true;
}


/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const tchar* InName, const tchar* InDefaultValue, const tchar* InHelpText /* = TEXT( "" ) */, uint32 InFlags /* = FCVAR_None */, ConVarChangeCallbackFn_t InChangeCallbackFn /* = nullptr */ )
	: CConCmdBase( InName, InHelpText, InFlags )
	, bHasMin( false )
	, bHasMax( false )
	, minValue( 0.f )
	, maxValue( 0.f )
	, intValue( 0 )
	, floatValue( 0.f )
	, defaultValue( InDefaultValue )
{
	SetString( defaultValue );
	if ( InChangeCallbackFn )
	{
		onChangeVar.Add( InChangeCallbackFn );
	}
}

/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const tchar* InName, const tchar* InDefaultValue, bool InHasMin, float InMin, bool InHasMax, float InMax, const tchar* InHelpText /* = TEXT( "" ) */, uint32 InFlags /* = FCVAR_None */, ConVarChangeCallbackFn_t InChangeCallbackFn /* = nullptr */ )
	: CConCmdBase( InName, InHelpText, InFlags )
	, bHasMin( bHasMin )
	, bHasMax( bHasMax )
	, minValue( InMin )
	, maxValue( InMax )
	, intValue( 0 )
	, floatValue( 0.f )
	, defaultValue( InDefaultValue )
{
	SetString( defaultValue );
	if ( InChangeCallbackFn )
	{
		onChangeVar.Add( InChangeCallbackFn );
	}
}

/*
==================
CConVar::IsCommand
==================
*/
bool CConVar::IsCommand() const
{
	return false;
}

/*
==================
CConVar::Reset
==================
*/
void CConVar::Reset()
{
	SetString( defaultValue );
}

/*
==================
CConVar::SetInt
==================
*/
void CConVar::SetInt( int32 InValue )
{
	if ( intValue != InValue )
	{
		floatValue = ( float )InValue;
		if ( ClampValue( floatValue ) )
		{
			intValue = ( int32 )floatValue;
		}
		else
		{
			intValue = InValue;
		}
		stringValue = L_Sprintf( TEXT( "%i" ), intValue );
		OnChangeValue();
	}
}

/*
==================
CConVar::SetFloat
==================
*/
void CConVar::SetFloat( float InValue )
{
	if ( floatValue != InValue )
	{
		floatValue	= InValue;
		ClampValue( floatValue );
		intValue	= ( int32 )floatValue;
		stringValue = L_Sprintf( TEXT( "%f" ), floatValue );
		OnChangeValue();
	}
}

/*
==================
CConVar::SetBool
==================
*/
void CConVar::SetBool( bool InValue )
{
	SetInt( InValue ? 1 : 0 );
}

/*
==================
CConVar::SetString
==================
*/
void CConVar::SetString( const tchar* InValue )
{
	if ( InValue != stringValue )
	{
		stringValue = InValue;
		if ( !InValue )
		{
			floatValue = 0.f;
		}
		else
		{
			floatValue = L_Atof( InValue );
		}

		if ( ClampValue( floatValue ) )
		{
			stringValue = L_Sprintf( TEXT( "%f" ), floatValue );
		}
		intValue = ( int32 )floatValue;
		OnChangeValue();
	}
}

/*
==================
CConVar::ClampValue
==================
*/
bool CConVar::ClampValue( float& InOutValue )
{
	if ( bHasMin && InOutValue < minValue )
	{
		InOutValue = minValue;
		return true;
	}

	if ( bHasMax && InOutValue > maxValue )
	{
		InOutValue = maxValue;
		return true;
	}

	return false;
}

/*
==================
CConVar::OnChangeValue
==================
*/
void CConVar::OnChangeValue()
{
	// Invoke callback function
	onChangeVar.Broadcast( this );
}


/*
==================
CConVarRef::Init
==================
*/
void CConVarRef::Init( const tchar* InName, bool InIsIgnoreMissing /* = false */ )
{
	// Try find cvar in the system
	conVar = g_Cvar.FindVar( InName );

	// If conVar isn't found print warning
	if ( !IsValid() && !InIsIgnoreMissing )
	{
		static bool		s_bFirst = true;
		if ( s_bFirst )
		{
			Warnf( TEXT( "CConVarRef '%s' doesn't point to an existing CConVar\n" ), InName );
			s_bFirst = false;
		}
	}
}