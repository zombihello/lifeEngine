#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptFunctionCompiler.h"
#include "Scripts/ScriptFunctionParser.h"
#include "Scripts/ScriptCodeGenerator.h"

/*
==================
CScriptFunctionCompiler::CScriptFunctionCompiler
==================
*/
CScriptFunctionCompiler::CScriptFunctionCompiler( const CScriptCompilerEnvironment& InEnvironment, bool InVerbose /* = false */ )
	: bVerbose( InVerbose )
	, environment( &InEnvironment )
{}

/*
==================
CScriptFunctionCompiler::Compile
==================
*/
bool CScriptFunctionCompiler::Compile( const CScriptClassStub& InClassStub, CScriptFunctionStub& InOutFunctionStub )
{
	// We must compile only script function and native one have to be implemented on C++ side
	AssertMsg( !InOutFunctionStub.IsNative(), TEXT( "We must compile only script functions" ) );

	// For script functions generate full code
	CScriptFunctionParser		functionParser;
	CScriptSyntaxNode_Base*		syntaxTree = functionParser.ParseFunctionCode( &InOutFunctionStub );

	// Compile code
	if ( syntaxTree )
	{
		// Show the tree
		if ( bVerbose )
		{
			Logf( TEXT( "Syntax tree:\n" ) );
			syntaxTree->Print();
			Logf( TEXT( "\n" ) );
		}

		// Generate bytecode from syntax tree
		CScriptCodeGenerator	codeGenerator( bVerbose );
		codeGenerator.GenerateCode( InClassStub, syntaxTree );

		// Copy bytecode into function
		InOutFunctionStub.GetCreatedFunction()->SetBytecode( codeGenerator.GetBytecode() );
	}

	delete syntaxTree;
	return true;
}