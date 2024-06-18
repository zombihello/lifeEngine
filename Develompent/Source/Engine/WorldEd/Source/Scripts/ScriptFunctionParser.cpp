#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptFunctionParser.h"

// Entry function grammar to parse a function source code
extern int LifeScript_GrammarFunction( CScriptTokenStream& InTokens, CScriptFunctionParser& InFunctionParser );

/*
==================
CScriptFunctionParser::CScriptFunctionParser
==================
*/
CScriptFunctionParser::CScriptFunctionParser()
	: bHasError( false )
	, rootSyntaxNode( nullptr )
{}

/*
==================
CScriptFunctionParser::ParseFunctionCode
==================
*/
CScriptSyntaxNode_Base* CScriptFunctionParser::ParseFunctionCode( CScriptFunctionStub* InFunctionStub )
{
	// Parse the function code
	int32	result = LifeScript_GrammarFunction( InFunctionStub->GetCode(), *this );
	
	// If there were errors we release syntax tree
	if ( result != 0 || bHasError )
	{
		if ( rootSyntaxNode )
		{
			delete rootSyntaxNode;
			rootSyntaxNode = nullptr;
		}
	}

	// Done, return parsed syntax node
	return rootSyntaxNode;
}

/*
==================
CScriptFunctionParser::EmitError
==================
*/
void CScriptFunctionParser::EmitError( const ScriptFileContext& InContext, const std::wstring& InMessage )
{
	Errorf( TEXT( "%s: %s\n" ), InContext.ToString().c_str(), InMessage.c_str() );
	bHasError = true;
}

/*
==================
CScriptFunctionParser::SetRootSyntaxNode
==================
*/
void CScriptFunctionParser::SetRootSyntaxNode( CScriptSyntaxNode_Base* InNode )
{
	if ( !rootSyntaxNode && InNode )
	{
		// Use only the first syntax tree
		rootSyntaxNode = InNode;
	}
	else if ( InNode )
	{
		delete InNode;
	}
}