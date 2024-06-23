#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptFunctionCompiler.h"
#include "Scripts/ScriptFunctionParser.h"
#include "Scripts/ScriptControlPathVisitor.h"
#include "Scripts/ScriptSyntaxCheckerVisitor.h"
#include "Scripts/ScriptSyntaxPrintVisitor.h"
#include "Scripts/ScriptCodeGeneratorVisitor.h"

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
	CScriptSyntaxNode_Base*		syntaxTree	= functionParser.ParseFunctionCode( &InOutFunctionStub );
	bool						bHasError	= !syntaxTree;

	// Compile code
	if ( syntaxTree )
	{
		// Show the syntax tree if it need
		if ( bShowDump )
		{
			Logf( TEXT( "%s: '%s:%s'\n" ), InOutFunctionStub.GetContext().ToString().c_str(), InClassStub.GetName().c_str(), InOutFunctionStub.GetName().c_str() );
			Logf( TEXT( "Syntax tree:\n" ) );
			CScriptSyntaxPrintVisitor		syntaxPrintVisitor;
			syntaxTree->AcceptVisitor( syntaxPrintVisitor );
			Logf( TEXT( "\n" ) );
		}

		// Check control paths
		bool							bHasReturnValue = !InOutFunctionStub.GetReturnType().IsVoid();
		CScriptControlPathVisitor		controlPathVisitor;
		syntaxTree->AcceptVisitor( controlPathVisitor );
		if ( controlPathVisitor.GetRootPathNode() )
		{
			bHasError = !controlPathVisitor.GetRootPathNode()->CheckControlPaths( bHasReturnValue );
		}
		else if ( bHasReturnValue )
		{
			Errorf( TEXT( "%s: Function must return a value\n" ), InOutFunctionStub.GetContext().ToString().c_str() );
			bHasError = true;
		}

		// Check on syntax errors
		CScriptSyntaxCheckerVisitor		syntaxCheckerVisitor( InClassStub, InOutFunctionStub );
		syntaxTree->AcceptVisitor( syntaxCheckerVisitor );
		bHasError |= syntaxCheckerVisitor.HasError();

		// Generate bytecode from syntax tree if we haven't any error
		if ( !bHasError )
		{
			// Generate code from syntax tree
			CScriptCodeGeneratorVisitor		codeGeneratorVisitor( InClassStub, InOutFunctionStub );
			syntaxTree->AcceptVisitor( codeGeneratorVisitor );
			CScriptCodeBlock*			codeBlocks = codeGeneratorVisitor.GetCodeBlocks();
			Assert( codeBlocks );

			// Dump code blocks to log
			if ( bShowDump )
			{
				Logf( TEXT( "Code blocks:\n" ) );
				codeBlocks->Print();
				Logf( TEXT( "\n" ) );
			}

			// Merge code blocks into one bytecode array
			std::vector<byte>		bytecode;
			for ( CScriptCodeBlock* currentCodeBlock = codeBlocks; currentCodeBlock; currentCodeBlock = currentCodeBlock->GetNext() )
			{
				uint32						offset = bytecode.size();
				const std::vector<byte>&	codeBlockBytecode = currentCodeBlock->GetBytecode();
				bytecode.resize( offset + codeBlockBytecode.size() );
				Memory::Memcpy( bytecode.data() + offset, codeBlockBytecode.data(), codeBlockBytecode.size() );
			}

			// Add opcode of the end
			bytecode.push_back( OP_Return );

			// Set bytecode to the function
			InOutFunctionStub.GetCreatedFunction()->SetBytecode( bytecode );
		}
	}

	delete syntaxTree;
	return !bHasError;
}