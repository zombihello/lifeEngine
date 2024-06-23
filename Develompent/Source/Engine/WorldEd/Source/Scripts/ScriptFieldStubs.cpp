#include "Scripts/ScriptFieldStubs.h"

/*
==================
CScriptBaseStub::CScriptBaseStub
==================
*/
CScriptBaseStub::CScriptBaseStub( const ScriptFileContext& InContext, const std::wstring& InName )
	: context( InContext )
	, name( InName )
{}


/*
==================
CScriptFunctionStub::CScriptFunctionStub
==================
*/
CScriptFunctionParamStub::CScriptFunctionParamStub( const ScriptFileContext& InContext, const std::wstring& InParamName, const ScriptFileContext& InTypeContext, const CScriptTypeDummy& InType )
	: CScriptBaseStub( InContext, InParamName )
	, typeContext( InTypeContext )
	, type( InType )
	, createdFuncParam( nullptr )
{}


/*
==================
CScriptFunctionStub::CScriptFunctionStub
==================
*/
CScriptFunctionStub::CScriptFunctionStub( const ScriptFileContext& InContext, const std::wstring& InFunctionName, const ScriptFileContext& InReturnTypeContext, const CScriptTypeDummy& InReturnType, uint32 InFlags )
	: CScriptBaseStub( InContext, InFunctionName )
	, bHasBody( false )
	, flags( InFlags )
	, returnTypeContext( InReturnTypeContext )
	, returnType( InReturnType )
	, createdFunction( nullptr )
{}


/*
==================
CScriptEnumStub::CScriptEnumStub
==================
*/
CScriptEnumStub::CScriptEnumStub( const ScriptFileContext& InContext, const std::wstring& InEnumName, bool InIsIntrinsic /* = false */ )
	: CScriptBaseStub( InContext, InEnumName )
	, bIntrinsic( InIsIntrinsic )
{}


/*
==================
CScriptStructStub::CScriptStructStub
==================
*/
CScriptStructStub::CScriptStructStub( const ScriptFileContext& InContext, const std::wstring& InStructName, bool InIsIntrinsic /* = false */ )
	: CScriptBaseStub( InContext, InStructName )
	, bIntrinsic( InIsIntrinsic )
{}


/*
==================
CScriptClassStub::CScriptClassStub
==================
*/
CScriptClassStub::CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, uint32 InFlags )
	: CScriptBaseStub( InContext, InClassName )
	, flags( InFlags )
	, withinClassContext( InContext )
	, createdClass( nullptr )
{}

/*
==================
CScriptClassStub::AddCppText
==================
*/
void CScriptClassStub::AddCppText( const std::wstring& InCppText )
{
	const tchar*	buffer = InCppText.data();
	uint32			size = InCppText.size();
	if ( size > 0 )
	{
		// Skip \r\n or \n at start
		if ( size >= 2 && ( buffer[0] == TEXT( '\r' ) && buffer[1] == TEXT( '\n' ) ) )
		{
			buffer	+= 2;
			size	-= 2;
		}
		else if ( buffer[0] == TEXT( '\n' ) )
		{
			buffer	+= 1;
			size	-= 1;
		}

		// Skip \r\n or \n at end
		if ( size >= 2 && ( buffer[size-2] == TEXT( '\r' ) && buffer[size-1] == TEXT( '\n' ) ) )
		{
			size -= 2;
		}
		else if ( buffer[size-1] == TEXT( '\n' ) )
		{
			size -= 1;
		}

		// Copy C++ text to our buffer
		uint32		offset = cppText.size();
		if ( offset > 0 )
		{
			cppText += TEXT( "\n\n" );
			offset	= cppText.size();
		}

		cppText.resize( offset + size );
		Memory::Memcpy( ( byte* )cppText.data() + offset * sizeof( tchar ), ( byte* )buffer, size * sizeof( tchar ) );
	}
}


/*
==================
CScriptSystemStub::CScriptSystemStub
==================
*/
CScriptSystemStub::CScriptSystemStub()
{}

/*
==================
CScriptSystemStub::CScriptSystemStub
==================
*/
CScriptSystemStub::CScriptSystemStub( const CScriptSystemStub& InOther )
	: classes( InOther.classes )
	, classesMap( InOther.classesMap )
	, structs( InOther.structs )
	, structsMap( InOther.structsMap )
	, enums( InOther.enums )
	, enumsMap( InOther.enumsMap )
{}