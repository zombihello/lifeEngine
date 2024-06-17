#include "Logger/LoggerMacros.h"
#include "Scripts/LexerEmitter.h"
#include "Scripts/ScriptTokenStream.h"
#include "Scripts/ScriptFileParser.h"

// Entry function to call lexer to tokenize source code
extern void LifeScript_Tokenize( const std::string& InSourceCode, CBaseLexerListener* InLexerListener );

// Entry function grammar to parse file source code
extern int LifeScript_GrammarFile( CScriptTokenStream& InTokens, CScriptFileParser& InFileParser );

/*
==================
CScriptFileParser::CScriptFileParser
==================
*/
CScriptFileParser::CScriptFileParser( CScriptSystemStub& OutStubs )
	: bHasError( false )
	, stubs( OutStubs )
{}

/*
==================
CScriptFileParser::ParseFile
==================
*/
bool CScriptFileParser::ParseFile( const std::wstring& InFileName, const std::string& InCode )
{
	// Setup token output list
	CScriptTokenStream		tokens;

	// Tokenize the code
	{
		CScriptTokenEater	tokenEater( tokens, InFileName );
		CLexerEmitter		lexerEmitter( tokenEater, InFileName );
		LifeScript_Tokenize( InCode, &lexerEmitter );
	}

	// Initialize file parser
	int32	result = LifeScript_GrammarFile( tokens, *this );

	// Did we parse stuff correctly?
	return result == 0 && !bHasError;
}

/*
==================
CScriptFileParser::EmitError
==================
*/
void CScriptFileParser::EmitError( const ScriptFileContext* InContext, const std::wstring& InMessage )
{
	Errorf( TEXT( "%s: %s\n" ), InContext->ToString().c_str(), InMessage.c_str() );
	bHasError = true;
}

/*
==================
CScriptFileParser::StartClass
==================
*/
void CScriptFileParser::StartClass( const ScriptFileContext* InContext, const ScriptFileContext* InSuperClassContext, const std::string_view& InClassName, const std::string_view& InClassSuperName, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for class" ) );
	AssertMsg( InSuperClassContext, TEXT( "Invalid context for super class" ) );
	AssertMsg( !InClassName.empty() && !InClassSuperName.empty(), TEXT( "Class name or super class name isn't valid" ) );
	currentClass = MakeSharedPtr<CScriptClassStub>( *InContext, ANSI_TO_TCHAR( InClassName.data() ), *InSuperClassContext, ANSI_TO_TCHAR( InClassSuperName.data() ), InFlags );
	stubs.AddClass( currentClass );
}

/*
==================
CScriptFileParser::StartClass
==================
*/
void CScriptFileParser::StartClass( const ScriptFileContext* InContext, const ScriptFileContext* InSuperClassContext, const ScriptFileContext* InWithinClassContext, const std::string_view& InClassName, const std::string_view& InClassSuperName, const std::string_view& InWithinClassName, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for class" ) );
	AssertMsg( InSuperClassContext, TEXT( "Invalid context for super class" ) );
	AssertMsg( InWithinClassContext, TEXT( "Invalid context for within class" ) );
	AssertMsg( !InClassName.empty() && !InClassSuperName.empty() && !InWithinClassName.empty(), TEXT( "Class name or super class name or within class name isn't valid" ) );
	currentClass = MakeSharedPtr<CScriptClassStub>( *InContext, ANSI_TO_TCHAR( InClassName.data() ), *InSuperClassContext, ANSI_TO_TCHAR( InClassSuperName.data() ), *InWithinClassContext, ANSI_TO_TCHAR( InWithinClassName.data() ), InFlags );
	stubs.AddClass( currentClass );
}

/*
==================
CScriptFileParser::AddCppText
==================
*/
void CScriptFileParser::AddCppText( const ScriptFileContext* InContext, const std::string_view& InCppText )
{
	AssertMsg( InContext, TEXT( "Invalid context for C++ text" ) );
	AssertMsg( currentClass, TEXT( "C++ text must be in class" ) );
	if ( !currentClass->HasAnyFlags( CLASS_Native ) )
	{
		Warnf( TEXT( "%s: In script class exist 'cpptext' but this is class isn't native and C++ code will be ignored\n" ), InContext->ToString().c_str() );
		return;
	}

	if ( !InCppText.empty() )
	{
		currentClass->AddCppText( ANSI_TO_TCHAR( InCppText.data() ) );
	}
}

/*
==================
CScriptFileParser::EndDefinition
==================
*/
void CScriptFileParser::EndDefinition( int32 InLine, const ScriptFileContext* InScopeStart, const ScriptFileContext* InScopeEnd )
{
	AssertMsg( InLine > -1, TEXT( "Invalid end line" ) );
	
	// Exit context
	ScriptScopeStub*	scope = nullptr;
	if ( currentFunction )
	{
		scope = &currentFunction->GetScope();
	}
	else if ( currentClass )
	{
		scope = &currentClass->GetScope();

	}

	AssertMsg( scope, TEXT( "No stub to end" ) );
	scope->startContext = *InScopeStart;
	scope->endContext	= *InScopeEnd;
}

/*
==================
CScriptFileParser::StartFunction
==================
*/
void CScriptFileParser::StartFunction( const ScriptFileContext* InContext, const ScriptFileContext* InReturnTypeContext, const std::string_view& InFunctionName, const std::string_view& InReturnTypeName, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for function" ) );
	AssertMsg( InReturnTypeContext, TEXT( "Invalid context for return type" ) );
	AssertMsg( !InFunctionName.empty() && !InReturnTypeName.empty(), TEXT( "Function name or return type name isn't valid" ) );

	// Create function
	if ( currentClass )
	{
		currentFunction = MakeSharedPtr<CScriptFunctionStub>( *InContext, ANSI_TO_TCHAR( InFunctionName.data() ), *InReturnTypeContext, ANSI_TO_TCHAR( InReturnTypeName.data() ), InFlags );
		currentClass->AddFunction( currentFunction );
	}
	else
	{
		EmitError( InContext, TEXT( "The Function must be in a class" ) );
	}
}

/*
==================
CScriptFileParser::GetFunctionCodeTokens
==================
*/
CScriptTokenStream& CScriptFileParser::GetFunctionCodeTokens()
{
	AssertMsg( currentFunction, TEXT( "No function defined" ) );
	AssertMsg( !currentFunction->HasAnyFlags( FUNC_Native ), TEXT( "Native function cannot have a code" ) );
	return currentFunction->GetCode();
}

/*
==================
CScriptFileParser::AddProperty
==================
*/
void CScriptFileParser::AddProperty( const ScriptFileContext* InContext, const ScriptFileContext* InTypeContext, const std::string_view& InPropertyName, const std::string_view& InTypeName, bool InIsFunctionParam )
{
	AssertMsg( InContext, TEXT( "Invalid context for property" ) );
	AssertMsg( InTypeContext, TEXT( "Invalid context for property type" ) );
	AssertMsg( false, TEXT( "Need implement" ) );
}

/*
==================
CScriptFileParser::PopContext
==================
*/
void CScriptFileParser::PopContext()
{
	// Exit context
	if ( currentFunction )
	{
		currentFunction = nullptr;
	}
	else if ( currentClass )
	{
		currentClass = nullptr;
	}
}