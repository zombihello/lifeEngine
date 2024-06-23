#include "Misc/StringConv.h"
#include "Scripts/ScriptTypeDummy.h"
#include "Scripts/ScriptTypeResolver.h"

/*
==================
CScriptTypeDummy::CScriptTypeDummy
==================
*/
CScriptTypeDummy::CScriptTypeDummy()
	: type( SDT_Void )
{}

/*
==================
CScriptTypeDummy::CScriptTypeDummy
==================
*/
CScriptTypeDummy::CScriptTypeDummy( const CScriptTypeDummy& InOther )
	: type( InOther.type )
	, identName( InOther.identName )
{}

/*
==================
CScriptTypeDummy::CScriptTypeDummy
==================
*/
CScriptTypeDummy::CScriptTypeDummy( CScriptTypeDummy&& InOther )
	: type( std::move( InOther.type ) )
	, identName( std::move( InOther.identName ) )
{}

/*
==================
CScriptTypeDummy::operator=
==================
*/
CScriptTypeDummy& CScriptTypeDummy::operator=( const CScriptTypeDummy& InOther )
{
	if ( this != &InOther )
	{
		type		= InOther.type;
		identName	= InOther.identName;
	}
	return *this;
}

/*
==================
CScriptTypeDummy::operator=
==================
*/
CScriptTypeDummy& CScriptTypeDummy::operator=( CScriptTypeDummy&& InOther )
{
	if ( this != &InOther )
	{
		type		= std::move( InOther.type );
		identName	= std::move( InOther.identName );
	}
	return *this;
}

/*
==================
CScriptTypeDummy::MakeVoid
==================
*/
CScriptTypeDummy CScriptTypeDummy::MakeVoid()
{
	return CScriptTypeDummy();
}

/*
==================
CScriptTypeDummy::MakeSimple
==================
*/
CScriptTypeDummy CScriptTypeDummy::MakeSimple( const std::string_view& InName )
{
	Assert( !InName.empty() );
	CScriptTypeDummy	result;
	result.type			= SDT_Simple;
	result.identName	= CScriptTypeResolver::TranslateBuiltInTypes( ANSI_TO_TCHAR( InName.data() ) );
	return result;
}

/*
==================
CScriptTypeDummy::ToString
==================
*/
std::wstring CScriptTypeDummy::ToString() const
{
	std::wstring	result;
	switch ( type )
	{
	case SDT_Void:		result = TEXT( "void" );	break;
	case SDT_Simple:	result = identName;			break;
	}

	return result;
}