#include "Reflection/ObjectGlobals.h"
#include "Scripts/ScriptTypeResolver.h"

//
// GLOBALS
//
bool																	CScriptTypeResolver::bBuiltInTypesInited = false;
std::unordered_map<CName, ScriptBuiltInType, CName::HashFunction>		CScriptTypeResolver::builtInTypes;


/*
==================
ScriptTypeResolveParams::ScriptTypeResolveParams
==================
*/
ScriptTypeResolveParams::ScriptTypeResolveParams( CObject* InOuter, const CName& InName, uint32 InObjectFlags, const CScriptTypeDummy& InDummyType, uint32 InOffset, uint32 InPropertyFlags, const CName& InCategory /* = NAME_None */, const std::wstring& InDescription /* = TEXT( "" ) */ )
	: dummyType( InDummyType )
	, outer( InOuter )
	, name( InName )
	, objectFlags( InObjectFlags )
	, offset( InOffset )
	, propertyFlags( InPropertyFlags )
	, category( InCategory )
	, description( InDescription )
{}


/*
==================
CScriptTypeResolver::Resolve
==================
*/
CProperty* CScriptTypeResolver::Resolve( const ScriptTypeResolveParams& InParams )
{
	// Void type
	if ( InParams.dummyType.IsVoid() )
	{
		return nullptr;
	}
	// Type given by name
	else if ( InParams.dummyType.IsSimple() )
	{
		const std::wstring&		typeName = InParams.dummyType.GetIdentName();
		AssertMsg( !typeName.empty(), TEXT( "Type name should not be empty for simple types" ) );

		// Is this a class
		if ( FindObjectFast<CClass>( nullptr, typeName.c_str(), true, true ) )
		{
			AssertMsg( false, TEXT( "Need implement" ) );
			return nullptr;
		}

		// Is this a struct
		if ( FindObjectFast<CStruct>( nullptr, typeName.c_str(), true, true ) )
		{
			AssertMsg( false, TEXT( "Need implement" ) );
			return nullptr;
		}

		// Is this an enum
		if ( FindObjectFast<CEnum>( nullptr, typeName.c_str(), true, true ) )
		{
			AssertMsg( false, TEXT( "Need implement" ) );
			return nullptr;
		}

		// Is this a built-in type
		if ( IsBuiltInType( typeName ) )
		{
			return builtInTypes[typeName].CreatePropertyFn( InParams );
		}
	}

	// Unresolvable type
	return nullptr;
}

/*
==================
CScriptTypeResolver::Resolve
==================
*/
std::wstring CScriptTypeResolver::Resolve( const CScriptTypeDummy& InDummyType )
{
	// Void type
	if ( InDummyType.IsVoid() )
	{
		return TEXT( "void" );
	}
	// Type given by name
	else if ( InDummyType.IsSimple() )
	{
		const std::wstring&		typeName = InDummyType.GetIdentName();
		AssertMsg( !typeName.empty(), TEXT( "Type name should not be empty for simple types" ) );

		// Is this a built-in type, class, struct or enum
		if ( IsBuiltInType( typeName ) || 
			 FindObjectFast<CClass>( nullptr, typeName.c_str(), true, true ) || 
			 FindObjectFast<CStruct>( nullptr, typeName.c_str(), true, true ) || 
			 FindObjectFast<CEnum>( nullptr, typeName.c_str(), true, true ) )
		{
			return typeName;
		}
	}

	// Unresolvable type
	return TEXT( "" );
}

/*
==================
CScriptTypeResolver::TranslateBuiltInTypes
==================
*/
std::wstring CScriptTypeResolver::TranslateBuiltInTypes( const std::wstring& InName )
{
	InitBuiltInTypes();
	for ( auto it = builtInTypes.begin(), itEnd = builtInTypes.end(); it != itEnd; ++it )
	{
		const ScriptBuiltInType&	builtInType = it->second;
		if ( builtInType.scriptName == InName )
		{
			return builtInType.name.ToString();
		}
	}

	return InName;
}

/*
==================
CScriptTypeResolver::InitBuiltInTypes
==================
*/
void CScriptTypeResolver::InitBuiltInTypes()
{
	// Do nothing if built-in types already is initialized
	if ( bBuiltInTypesInited )
	{
		return;
	}

	// Int type
	AddBuiltInType( TEXT( "int32" ), TEXT( "int" ), 
					[]( const ScriptTypeResolveParams& InParams ) -> CProperty* 
					{
						return new( InParams.outer, InParams.name, InParams.objectFlags ) CIntProperty( InParams.offset, InParams.category, InParams.description, InParams.propertyFlags );
					} );

	// Float type
	AddBuiltInType( TEXT( "float" ), TEXT( "float" ),
					[]( const ScriptTypeResolveParams& InParams ) -> CProperty*
					{
						return new( InParams.outer, InParams.name, InParams.objectFlags ) CFloatProperty( InParams.offset, InParams.category, InParams.description, InParams.propertyFlags );
					} );

	// We are done
	bBuiltInTypesInited = true;
}