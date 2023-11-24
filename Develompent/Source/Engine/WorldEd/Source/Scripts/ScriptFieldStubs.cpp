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
CScriptFunctionParamStub::CScriptFunctionParamStub
==================
*/
CScriptFunctionParamStub::CScriptFunctionParamStub( const ScriptFileContext& InContext, const std::wstring& InParamName, const ScriptFileContext& InTypeContext, const std::wstring& InTypeName )
	: CScriptBaseStub( InContext, InParamName )
	, typeContext( InTypeContext )
	, typeName( InTypeName)
{}


/*
==================
CScriptFunctionStub::CScriptFunctionStub
==================
*/
CScriptFunctionStub::CScriptFunctionStub( const ScriptFileContext& InContext, const std::wstring& InFunctionName, const ScriptFileContext& InReturnTypeContext, const std::wstring& InReturnTypeName, uint32 InFlags )
	: CScriptBaseStub( InContext, InFunctionName )
	, bIsNative( InFlags & SSF_Native )
	, returnTypeContext( InReturnTypeContext )
	, returnTypeName( InReturnTypeName )
	, createdFunction( nullptr )
{}


/*
==================
CScriptClassStub::CScriptClassStub
==================
*/
CScriptClassStub::CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName, uint32 InFlags )
	: CScriptBaseStub( InContext, InClassName )
	, bIsNative( InFlags& SSF_Native )
	, superClassContext( InSuperClassContext )
	, superClassName( InSuperClassName )
{}


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
{
	classes = InOther.classes;
}