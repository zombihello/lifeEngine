#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptCodeGenerator.h"

/*
==================
CScriptCodeGenerator::GenerateCode
==================
*/
void CScriptCodeGenerator::GenerateCode( const CScriptClassStub& InClassStub, CScriptSyntaxNode_Base* InSyntaxTree, bool InIsShowDump /* = false */ )
{
	// Clear bytecode array
	bytecode.clear();

	// Generate code from syntax tree
	Assert( InSyntaxTree );
	CScriptClassStub*	currentContext	= ( CScriptClassStub* )&InClassStub;
	CScriptCodeBlock*	codeBlocks		= InSyntaxTree->GenerateCode( currentContext );
	Assert( codeBlocks );

	// Dump code blocks to log if it need
	if ( InIsShowDump )
	{
		Logf( TEXT( "Code blocks:\n" ) );
		codeBlocks->Print();
	}

	// Merge code blocks' bytecode to one
	for ( CScriptCodeBlock* currentCodeBlock = codeBlocks; currentCodeBlock; currentCodeBlock = currentCodeBlock->GetNext() )
	{
		uint32						offset = bytecode.size();
		const std::vector<byte>&	codeBlockBytecode = currentCodeBlock->GetBytecode();
		bytecode.resize( offset + codeBlockBytecode.size() );
		Memory::Memcpy( bytecode.data() + offset, codeBlockBytecode.data(), codeBlockBytecode.size() );
	}

	// Add opcode of the end
	bytecode.push_back( OP_Return );

	// Free allocated memory
	delete codeBlocks;
}