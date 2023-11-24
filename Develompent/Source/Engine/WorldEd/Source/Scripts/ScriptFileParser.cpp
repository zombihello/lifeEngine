#include "Logger/LoggerMacros.h"
#include "Misc/SharedPointer.h"
#include "Scripts/LexerEmitter.h"
#include "Scripts/ScriptFileParser.h"

// Entry function to call lexer to tokenize a source code
extern void LifeScript_Tokenize( const std::string& InSourceCode, CBaseLexerListener* InLexerListener );

// Entry function grammar to parse a file source code
extern int LifeScript_GrammarFile( CScriptTokenStream& InTokens, CScriptFileParser& InFileParser );

/*
==================
CScriptFileParser::CScriptFileParser
==================
*/
CScriptFileParser::CScriptFileParser( CScriptSystemStub& OutStubs )
	: stubs( &OutStubs )
	, bHasError( false )
{}

/*
==================
CScriptFileParser::ParseFile
==================
*/
bool CScriptFileParser::ParseFile( const std::wstring& InFileName, const std::string& InCode, bool IsVerboseTokenCapture )
{
	// Setup token output list
	CScriptTokenStream		tokens;

	// Tokenize the code
	{
		TSharedPtr<CScriptTokenEater>	tokenListener = MakeSharedPtr<CScriptTokenEater>( tokens, ScriptStringTCHARBuf_t( InFileName ) );
		CLexerEmitter					lexerEmitter( tokenListener.Get(), InFileName );
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
	stubs->AddClass( currentClass );
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
void CScriptFileParser::StartFunction( const ScriptFileContext* InContext, const ScriptFileContext* InReturnTypeContext, const std::string& InFunctionName, const std::string& InReturnTypeName, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for function" ) );
	AssertMsg( InReturnTypeContext, TEXT( "Invalid context for return type" ) );
	
	// Create function
	if ( currentClass )
	{
		currentFunction = MakeSharedPtr<CScriptFunctionStub>( *InContext, ANSI_TO_TCHAR( InFunctionName.c_str() ), *InReturnTypeContext, ANSI_TO_TCHAR( InReturnTypeName.c_str() ), InFlags );
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
	AssertMsg( !currentFunction->IsNative(), TEXT( "Native function cannot have a code" ) );
	return currentFunction->GetCode();
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
	
	// Add the property to current function
	if ( currentFunction )
	{
		currentFunction->AddParam( MakeSharedPtr<CScriptFunctionParamStub>( *InContext, ANSI_TO_TCHAR( InPropertyName.c_str() ), *InTypeContext, ANSI_TO_TCHAR( InTypeName.c_str() ) ) );
	}
	// Otherwise we add to current class
	else if ( currentClass )
	{
		AssertMsg( false, TEXT( "Need implement" ) );
	}
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