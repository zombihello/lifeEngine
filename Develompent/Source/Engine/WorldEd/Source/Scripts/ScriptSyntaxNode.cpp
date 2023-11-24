#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptFieldStubs.h"
#include "Misc/ScriptOpcodes.h"
#include "System/MemoryArchive.h"

/*
==================
CScriptSyntaxNode_Nop::Print
==================
*/
void CScriptSyntaxNode_Nop::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "ScriptParser: %p(% 4i):%s Nop\n" ), this, context.line, trailingSpace.c_str() );
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_Nop::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Nop::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For nop operator we generate empty code
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( context, TEXT( "Nop" ) );
	codeBlock->GetBytecode().push_back( OP_Nop );
	return codeBlock;
}


/*
==================
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem
==================
*/
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InListRoot )
	: CScriptSyntaxNode_Base( SSNT_ListItem, InContext )
{
	MergeNode( InListRoot );
}

/*
==================
CScriptSyntaxNode_ListItem::~CScriptSyntaxNode_ListItem
==================
*/
CScriptSyntaxNode_ListItem::~CScriptSyntaxNode_ListItem()
{
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		delete nodes[index];
	}
}

/*
==================
CScriptSyntaxNode_ListItem::MergeNode
==================
*/
void CScriptSyntaxNode_ListItem::MergeNode( CScriptSyntaxNode_Base* InNode )
{
	// If node is valid we add it to list
	if ( InNode )
	{
		if ( InNode->GetType() == SSNT_ListItem )
		{
			CScriptSyntaxNode_ListItem*		listNode = ( CScriptSyntaxNode_ListItem* )InNode;
			nodes.reserve( listNode->GetNumNodes() );
			for ( uint32 index = 0, count = listNode->GetNumNodes(); index < count; ++index )
			{
				MergeNode( listNode->nodes[index] );
			}

			listNode->nodes.clear();
			delete listNode;
		}
		else
		{
			InNode->SetParentNode( this );
			nodes.push_back( InNode );
		}
	}
}

/*
==================
CScriptSyntaxNode_ListItem::Print
==================
*/
void CScriptSyntaxNode_ListItem::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );

	Logf( TEXT( "ScriptParser: %p(% 4i):%s ListItem (num: %i)\n" ), this, context.line, trailingSpace.c_str(), nodes.size() );
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		nodes[index]->Print( InPrintLevel + 2 );
	}
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_ListItem::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_ListItem::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For nop operator we generate empty code
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( context, TEXT( "ListItem" ) );
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		CScriptCodeBlock*	subCodeBlock = nodes[index]->GenerateCode( InCurrentContext );
		CScriptCodeBlock::Glue( codeBlock, subCodeBlock );
	}

	return codeBlock;
}


/*
==================
CScriptSyntaxNode_StringConst::Print
==================
*/
void CScriptSyntaxNode_StringConst::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "ScriptParser: %p(% 4i):%s StringConst (str: %s)\n" ), this, context.line, trailingSpace.c_str(), ANSI_TO_TCHAR( value.c_str() ) );
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_StringConst::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_StringConst::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	Assert( false );
	return nullptr;
}


/*
==================
CScriptSyntaxNode_ThisValue::Print
==================
*/
void CScriptSyntaxNode_ThisValue::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "ScriptParser: %p(% 4i):%s ThisValue\n" ), this, context.line, trailingSpace.c_str() );
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_ThisValue::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_ThisValue::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	Assert( false );
	return nullptr;
}


/*
==================
CScriptSyntaxNode_ScopeSuper::Print
==================
*/
void CScriptSyntaxNode_ScopeSuper::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "ScriptParser: %p(% 4i):%s ScopeSuper\n" ), this, context.line, trailingSpace.c_str() );
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_ScopeSuper::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_ScopeSuper::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	Assert( false );
	return nullptr;
}


/*
==================
CScriptSyntaxNode_Ident::Print
==================
*/
void CScriptSyntaxNode_Ident::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "ScriptParser: %p(% 4i):%s Ident (name: %s)\n" ), this, context.line, trailingSpace.c_str(), ANSI_TO_TCHAR( name.c_str() ) );
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_Ident::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Ident::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	bool				bHasError = false;
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( context, TEXT( "Ident" ) );
	CMemoryWriter		codeWriter( codeBlock->GetBytecode() );
	if ( parentNode )
	{
		switch ( parentNode->GetType() )
		{
			// Generate code block for func call
		case SSNT_FuncCall:
		{
			CClass*		currentClass = InCurrentContext->GetCreatedClass();
			Assert( currentClass );
			CFunction*	function = currentClass->FindFunction( ANSI_TO_TCHAR( name.c_str() ) );
			Assert( function );
			codeWriter << ( uptrint )function;
			break;
		}

		default:	
			bHasError = true;
			break;
		}
	}
	else
	{
		bHasError = true;
	}

	Assert( !bHasError );
	return codeBlock;
}


/*
==================
CScriptSyntaxNode_FuncCall::~CScriptSyntaxNode_FuncCall
==================
*/
CScriptSyntaxNode_FuncCall::~CScriptSyntaxNode_FuncCall()
{
	if ( nameNode )
	{
		delete nameNode;
	}

	if ( parametersNode )
	{
		delete parametersNode;
	}
}

/*
==================
CScriptSyntaxNode_FuncCall::Print
==================
*/
void CScriptSyntaxNode_FuncCall::Print( uint32 InPrintLevel /* = 0 */ )
{
#if !NO_LOGGING
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );

	Logf( TEXT( "ScriptParser: %p(% 4i):%s FuncCall\n" ), this, context.line, trailingSpace.c_str() );
	if ( nameNode )
	{
		nameNode->Print( InPrintLevel + 2 );
	}
	if ( parametersNode )
	{
		parametersNode->Print( InPrintLevel + 2 );
	}
#endif // !NO_LOGGING
}

/*
==================
CScriptSyntaxNode_FuncCall::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_FuncCall::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// Generate code for call a function
	TGuardValue<CScriptClassStub*>		currentContextGuard( InCurrentContext );
	CScriptCodeBlock*					codeBlock = new CScriptCodeBlock( context, TEXT( "FuncCall" ) );
	codeBlock->GetBytecode().push_back( OP_Call );

	// Generate code for function name and glue it	
	CScriptCodeBlock*					codeBlockFuncIdent = nameNode->GenerateCode( InCurrentContext );
	CScriptCodeBlock::Glue( codeBlock, codeBlockFuncIdent );

	// Currently parameters not supported
	Assert( !parametersNode );
	return codeBlock;
}