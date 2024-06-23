#include "Logger/LoggerMacros.h"
#include "Reflection/Object.h"
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
void CScriptFileParser::StartClass( const ScriptFileContext* InContext, const ScriptFileContext* InSuperClassContext, const ScriptFileContext* InWithinClassContext, const ScriptFileContext* InNativeClassGroupContext, const std::string_view& InClassName, const std::string_view& InClassSuperName, const std::string_view& InWithinClassName, const std::string_view& InNativeClassGroup, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for class" ) );
	AssertMsg( !InClassName.empty(), TEXT( "Class name isn't valid" ) );

	// Create a new class
	std::wstring	className		= ANSI_TO_TCHAR( InClassName.data() );
	bool			bIsCObjectClass = className == CObject::StaticClass()->GetName();
	currentClass	= MakeSharedPtr<CScriptClassStub>( *InContext, className, InFlags );

	// Make sure that CObject don't have a super class and within class
	if ( bIsCObjectClass )
	{
		// CObject should not have a super class, because it is the root class in the hierarchy
		if ( InSuperClassContext )
		{
			EmitError( InSuperClassContext, TEXT( "CObject should not have a super class, because it is the root class in the hierarchy" ) );
		}

		// Using 'within' with CObject class not allowed
		if ( InWithinClassContext )
		{
			EmitError( InWithinClassContext, TEXT( "Using 'within' with CObject class not allowed" ) );
		}
	}

	// Make sure that all classes have a super class (of course except CObject)
	if ( !bIsCObjectClass && !InSuperClassContext )
	{
		// Only CObject class can not have a super class
		EmitError( InContext, TEXT( "Only CObject class can not have a super class, other classes must inherit by the one" ) );
	}

	// Set a super class if it we have
	if ( InSuperClassContext )
	{
		AssertMsg( !InClassSuperName.empty(), TEXT( "Super class name isn't valid" ) );
		currentClass->SetSuperClass( *InSuperClassContext, ANSI_TO_TCHAR( InClassSuperName.data() ) );
	}

	// Set a within class if it we have
	if ( InWithinClassContext )
	{
		AssertMsg( !InWithinClassName.empty(), TEXT( "Within class name isn't valid" ) );
		currentClass->SetWithinClass( *InWithinClassContext, ANSI_TO_TCHAR( InWithinClassName.data() ) );
	}

	// Set a native class group if it we have
	if ( InNativeClassGroupContext )
	{
		AssertMsg( !InNativeClassGroup.empty(), TEXT( "Native class group isn't valid" ) );
		AssertMsg( currentClass->HasAnyFlags( CLASS_Native ), TEXT( "Invalid class flags. If was used 'native( .. )' must be set CLASS_Native" ) );
		currentClass->SetNativeClassGroup( *InNativeClassGroupContext, ANSI_TO_TCHAR( InNativeClassGroup.data() ) );
	}

	// Add a new class in the stubs system
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

	// C++ text must be in class body
	if ( !currentClass )
	{
		EmitError( InContext, TEXT( "'cpptext' must be in class body" ) );
		return;
	}

	// Print warning if 'cpptext' was used in script class
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

		// We check that the native function doesn't have a body
		if ( currentFunction->HasAnyFlags( FUNC_Native ) && currentFunction->HasBody() )
		{
			EmitError( InScopeStart, TEXT( "Native function cannot have a code" ) );
		}
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
void CScriptFileParser::StartFunction( const ScriptFileContext* InContext, const ScriptFileContext* InReturnTypeContext, const std::string_view& InFunctionName, const CScriptTypeDummy& InReturnType, uint32 InFlags )
{
	AssertMsg( InContext, TEXT( "Invalid context for function" ) );
	AssertMsg( InReturnTypeContext, TEXT( "Invalid context for return type" ) );
	AssertMsg( !InFunctionName.empty(), TEXT( "Function name isn't valid" ) );

	// Create function
	if ( currentClass )
	{
		currentFunction = MakeSharedPtr<CScriptFunctionStub>( *InContext, ANSI_TO_TCHAR( InFunctionName.data() ), *InReturnTypeContext, InReturnType, InFlags );
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

	// Mark that the function has body and return reference to token stream of function code
	currentFunction->SetHasBody( true );
	return currentFunction->GetCode();
}

/*
==================
CScriptFileParser::AddProperty
==================
*/
void CScriptFileParser::AddProperty( const ScriptFileContext* InContext, const ScriptFileContext* InTypeContext, const std::string_view& InPropertyName, const CScriptTypeDummy& InPropertyType, bool InIsFunctionParam )
{
	AssertMsg( InContext, TEXT( "Invalid context for property" ) );
	AssertMsg( InTypeContext, TEXT( "Invalid context for property type" ) );
	AssertMsg( !InPropertyName.empty(), TEXT( "Property name isn't valid" ) );

	// Add the property to current function
	if ( currentFunction )
	{
		// If our property is function parameter we add the one as parameter
		if ( InIsFunctionParam )
		{
			currentFunction->AddParam( MakeSharedPtr<CScriptFunctionParamStub>( *InContext, ANSI_TO_TCHAR( InPropertyName.data() ), *InTypeContext, InPropertyType ) );
		}
		// Otherwise is local function parameter
		else
		{
			AssertMsg( false, TEXT( "Need implement" ) );
		}
	}
	// Otherwise we add to current class
	else if ( currentClass )
	{
		AssertMsg( false, TEXT( "Need implement" ) );
	}
	else
	{
		EmitError( InContext, TEXT( "Properties can be only in functions or classes" ) );
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

/*
==================
CScriptFileParser::SetFunctionUndefined
==================
*/
void CScriptFileParser::SetFunctionUndefined()
{
	AssertMsg( currentFunction, TEXT( "This function cannot be called if a function is not being declared" ) );
	currentFunction->SetHasBody( false );
}