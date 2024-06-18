#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptFunctionCompiler.h"
#include "Scripts/ScriptFunctionParser.h"
#include "Scripts/ScriptCodeGenerator.h"

/*
==================
CScriptFunctionCompiler::CScriptFunctionCompiler
==================
*/
CScriptFunctionCompiler::CScriptFunctionCompiler( const CScriptSystemStub& InStubs, bool InIsShowDump /* = false */ )
	: bShowDump( InIsShowDump )
	, stubs( InStubs )
{}

/*
==================
CScriptFunctionCompiler::Compile
==================
*/
bool CScriptFunctionCompiler::Compile( const CScriptClassStub& InClassStub, CScriptFunctionStub& InOutFunctionStub )
{
	// We must compile only script function and native one have to be implemented on C++ side
	AssertMsg( !InOutFunctionStub.HasAnyFlags( FUNC_Native ), TEXT( "We must compile only script functions" ) );

	// For script functions generate full code
	CScriptFunctionParser		functionParser;
	CScriptSyntaxNode_Base*		syntaxTree = functionParser.ParseFunctionCode( &InOutFunctionStub );

	// Compile code
	if ( syntaxTree )
	{
		// Show the syntax tree if it need
		if ( bShowDump )
		{
			Logf( TEXT( "%s: '%s:%s'\n" ), InOutFunctionStub.GetContext().ToString().c_str(), InClassStub.GetName().c_str(), InOutFunctionStub.GetName().c_str() );
			Logf( TEXT( "Syntax tree:\n" ) );
			syntaxTree->Print();
			Logf( TEXT( "\n" ) );
		}

		// Generate bytecode from syntax tree
		CScriptCodeGenerator	codeGenerator;
		codeGenerator.GenerateCode( InClassStub, syntaxTree, bShowDump );

		if ( bShowDump )
		{
			Logf( TEXT( "\n" ) );
		}

		// Copy bytecode into function
		InOutFunctionStub.GetCreatedFunction()->SetBytecode( codeGenerator.GetBytecode() );
	}

	delete syntaxTree;
	return true;
}