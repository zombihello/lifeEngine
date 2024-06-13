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
CScriptClassStub::CScriptClassStub
==================
*/
CScriptClassStub::CScriptClassStub( const ScriptFileContext& InContext, const std::wstring& InClassName, const ScriptFileContext& InSuperClassContext, const std::wstring& InSuperClassName, uint32 InFlags )
	: CScriptBaseStub( InContext, InClassName )
	, flags( InFlags )
	, superClassContext( InSuperClassContext )
	, superClassName( InSuperClassName )
	, createdClass( nullptr )
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
	: classes( InOther.classes )
	, classesMap( InOther.classesMap )
{}