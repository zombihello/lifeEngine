#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptCodeBlock.h"

/*
==================
CScriptCodeBlock::CScriptCodeBlock
==================
*/
CScriptCodeBlock::CScriptCodeBlock( const ScriptFileContext& InContext, const std::wstring InDebugName /* = TEXT( "" ) */ )
	: debugName( InDebugName )
	, context( InContext )
	, nextCodeBlock( nullptr )
	, lastCodeBlock( nullptr )
{}

/*
==================
CScriptCodeBlock::~CScriptCodeBlock
==================
*/
CScriptCodeBlock::~CScriptCodeBlock()
{
	if ( nextCodeBlock )
	{
		delete nextCodeBlock;
	}
}

/*
==================
CScriptCodeBlock::Glue
==================
*/
void CScriptCodeBlock::Glue( CScriptCodeBlock* InNodeRoot, CScriptCodeBlock* InNextMode )
{
	Assert( InNodeRoot && InNextMode );
	if ( InNodeRoot->nextCodeBlock )
	{
		InNodeRoot->lastCodeBlock->nextCodeBlock	= InNextMode;
		InNodeRoot->lastCodeBlock					= InNextMode->lastCodeBlock ? InNextMode->lastCodeBlock : InNextMode;
	}
	else
	{
		InNodeRoot->nextCodeBlock = InNextMode;
		InNodeRoot->lastCodeBlock = InNextMode->lastCodeBlock ? InNextMode->lastCodeBlock : InNextMode;
	}
}

/*
==================
CScriptCodeBlock::Print
==================
*/
void CScriptCodeBlock::Print() const
{
	for ( const CScriptCodeBlock* currentCodeBlock = this; currentCodeBlock; currentCodeBlock = currentCodeBlock->nextCodeBlock )
	{
		std::wstring				bytecodeInString;
		const std::vector<byte>&	localCodebyte = currentCodeBlock->GetBytecode();
		for ( uint32 index = 0, count = localCodebyte.size(); index < count; ++index )
		{
			bytecodeInString += CString::Format( TEXT( "0x%02X" ), localCodebyte[index] );
			if ( index + 1 < count )
			{
				bytecodeInString += TEXT( ", " );
			}
		}

		Logf( TEXT( "CodeBlock: %p(% 4i): %s\t{ %s }\n" ), currentCodeBlock, currentCodeBlock->GetContext().line, currentCodeBlock->GetDebugName().c_str(), bytecodeInString.c_str() );
	}
}