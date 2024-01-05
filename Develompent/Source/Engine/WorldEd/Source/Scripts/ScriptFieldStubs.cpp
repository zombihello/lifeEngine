#include "Scripts/ScriptFieldStubs.h"

/** Table of script property type names for convert from Text into EScriptPropertyType */
static const tchar* s_ScriptPropertyTypeNames[] =
{
	TEXT( "" ),			/**< SPT_Unknown */
	TEXT( "BYTE" ),		/**< SPT_Byte */
	TEXT( "ENUM" ),		/**< SPT_Enum */
	TEXT( "INT" ),		/**< SPT_Int */
	TEXT( "BOOL" ),		/**< SPT_Bool */
	TEXT( "FLOAT" ),	/**< SPT_Float */
	TEXT( "COLOR" ),	/**< SPT_Color */
	TEXT( "OBJECT" ),	/**< SPT_Object */
	TEXT( "VECTOR" ),	/**< SPT_Vector */
	TEXT( "ROTATOR" ),	/**< SPT_Rotator */
	TEXT( "ASSET" ),	/**< SPT_Asset */
	TEXT( "ARRAY" ),	/**< SPT_Array */
	TEXT( "STRUCT" ),	/**< SPT_Struct */
	TEXT( "STRING" ),	/**< SPT_String */
};
static_assert( ARRAY_COUNT( s_ScriptPropertyTypeNames ) == SPT_Num, "Need full init s_ScriptPropertyTypeNames array" );

/*
==================
ConvertTextToEScriptPropertyType
==================
*/
EScriptPropertyType ConvertTextToEScriptPropertyType( const std::wstring& InString )
{
	std::wstring	string = CString::ToUpper( InString );
	for ( uint32 index = 0; index < SPT_Num; ++index )
	{
		if ( string == s_ScriptPropertyTypeNames[index] )
		{
			return ( EScriptPropertyType )index;
		}
	}

	return SPT_Unknown;
}


/*
==================
CScriptBaseStub::CScriptBaseStub
==================
*/
CScriptBaseStub::CScriptBaseStub( const std::wstring& InName )
	: name( InName )
{}


/*
==================
CScriptPropertyStub::CScriptPropertyStub
==================
*/
CScriptPropertyStub::CScriptPropertyStub( const std::wstring& InPropertyName, EScriptPropertyType InPropertyType, uint32 InFlags /* = CPF_None */, const std::wstring& InPropertySubType /* = TEXT( "" ) */ )
	: CScriptBaseStub( InPropertyName )
	, type( InPropertyType )
	, subType( InPropertySubType )
	, createdProperty( nullptr )
	, flags( InFlags )
	, arraySize( 1 )
{}


/*
==================
CScriptClassStub::CScriptClassStub
==================
*/
CScriptClassStub::CScriptClassStub( const std::wstring& InClassName, const std::wstring& InSuperClassName, uint32 InFlags /* = CLASS_None */ )
	: CScriptBaseStub( InClassName )
	, superClassName( InSuperClassName )
	, createdClass( nullptr )
{}


/*
==================
CScriptSystemStub::CScriptSystemStub
==================
*/
CScriptSystemStub::CScriptSystemStub( CObjectPackage* InPackage )
	: package( InPackage )
{
	Assert( package );
}