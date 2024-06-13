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
	int32	result = -1;
	{
		result = LifeScript_GrammarFile( tokens, *this );
	}

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
void CScriptFileParser::StartClass( const ScriptFileContext* InContext, const ScriptFileContext* InSuperClassContext, const std::string& InClassName, const std::string& InClassSuperName, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for class" ) );
	AssertMsg( InSuperClassContext, TEXT( "Invalid context for super class" ) );	
	currentClass = MakeSharedPtr<CScriptClassStub>( *InContext, ANSI_TO_TCHAR( InClassName.c_str() ), *InSuperClassContext, ANSI_TO_TCHAR( InClassSuperName.c_str() ), InFlags );
	stubs.AddClass( currentClass );
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
	if ( currentClass )
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
void CScriptFileParser::StartFunction( const ScriptFileContext* InContext, const ScriptFileContext* InReturnTypeContext, const std::string& InFunctionName, const std::string& InReturnTypeName, uint32 InFlags )
{}

/*
==================
CScriptFileParser::GetFunctionCodeTokens
==================
*/
CScriptTokenStream& CScriptFileParser::GetFunctionCodeTokens()
{
	static CScriptTokenStream dummy;
	return dummy;
}

/*
==================
CScriptFileParser::AddProperty
==================
*/
void CScriptFileParser::AddProperty( const ScriptFileContext* InContext, const ScriptFileContext* InTypeContext, const std::string& InPropertyName, const std::string& InTypeName, bool InIsFunctionParam )
{
	AssertMsg( InContext, TEXT( "Invalid context for property" ) );
	AssertMsg( InTypeContext, TEXT( "Invalid context for property type" ) );
}

/*
==================
CScriptFileParser::PopContext
==================
*/
void CScriptFileParser::PopContext()
{
	// Exit context
	if ( currentClass )
	{
		currentClass = nullptr;
	}
}