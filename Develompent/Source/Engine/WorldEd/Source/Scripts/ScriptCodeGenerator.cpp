#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptCodeGenerator.h"
#include "Scripts/ScriptFieldStubs.h"

/*
==================
CScriptCodeGenerator::CScriptCodeGenerator
==================
*/
CScriptCodeGenerator::CScriptCodeGenerator( bool InVerbose /* = false */ )
	: bVerbose( InVerbose )
{}

/*
==================
CScriptCodeGenerator::GenerateCode
==================
*/
void CScriptCodeGenerator::GenerateCode( const CScriptClassStub& InClassStub, CScriptSyntaxNode_Base* InSyntaxTree )
{
	// Clear bytecode array
	bytecode.clear();

	// Generate code from syntax tree
	Assert( InSyntaxTree );
	CScriptClassStub*	currentContext = ( CScriptClassStub* )&InClassStub;
	CScriptCodeBlock*	codeBlocks = InSyntaxTree->GenerateCode( currentContext );
	Assert( codeBlocks );

	// Dump code blocks to log
	if ( bVerbose )
	{
		Logf( TEXT( "Code blocks:\n" ) );
		codeBlocks->Print();
		Logf( TEXT( "\n" ) );
	}

	// Merge code blocks' bytecode to one
	for ( CScriptCodeBlock* currentCodeBlock = codeBlocks; currentCodeBlock; currentCodeBlock = currentCodeBlock->GetNext() )
	{
		uint32						offset = bytecode.size();
		const std::vector<byte>&	codeBlockBytecode = currentCodeBlock->GetBytecode();
		bytecode.resize( offset + codeBlockBytecode.size() );
		memcpy( bytecode.data() + offset, codeBlockBytecode.data(), codeBlockBytecode.size() );
	}

	// Add opcode of the end
	bytecode.push_back( OP_Return );

	// Free allocated memory
	delete codeBlocks;
}