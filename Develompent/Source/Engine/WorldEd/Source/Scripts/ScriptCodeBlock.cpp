#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptCodeBlock.h"

/*
==================
CScriptCodeBlock::CScriptCodeBlock
==================
*/
CScriptCodeBlock::CScriptCodeBlock( const ScriptFileContext& InContext, const CName& InDebugName /* = NAME_None */ )
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
void CScriptCodeBlock::Glue( CScriptCodeBlock* InNodeRoot, CScriptCodeBlock* InNodeNext )
{
	Assert( InNodeRoot && InNodeNext );
	if ( InNodeRoot->nextCodeBlock )
	{
		InNodeRoot->lastCodeBlock->nextCodeBlock	= InNodeNext;
		InNodeRoot->lastCodeBlock					= InNodeNext->lastCodeBlock ? InNodeNext->lastCodeBlock : InNodeNext;
	}
	else
	{
		InNodeRoot->nextCodeBlock					= InNodeNext;
		InNodeRoot->lastCodeBlock					= InNodeNext->lastCodeBlock ? InNodeNext->lastCodeBlock : InNodeNext;
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
		const std::vector<byte>&	localByteCode = currentCodeBlock->GetBytecode();
		for ( uint32 index = 0, count = localByteCode.size(); index < count; ++index )
		{
			bytecodeInString += L_Sprintf( TEXT( "0x%02X%s" ), localByteCode[index], index + 1 < count ? TEXT( ", " ) : TEXT( "" ) );
		}

		Logf( TEXT( "%p(%4i): %s\t{ %s }\n" ), currentCodeBlock, currentCodeBlock->GetContext().line, currentCodeBlock->GetDebugName().ToString().c_str(), bytecodeInString.c_str() );
	}
}