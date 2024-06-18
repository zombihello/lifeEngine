#include "Logger/LoggerMacros.h"
#include "System/MemoryArchive.h"
#include "Scripts/ScriptSyntaxNode.h"

/*
==================
CScriptSyntaxNode_Base::CScriptSyntaxNode_Base
==================
*/
CScriptSyntaxNode_Base::CScriptSyntaxNode_Base( EScriptSyntaxNodeType InType, const ScriptFileContext& InContext )
	: type( InType )
	, context( InContext )
	, parentNode( nullptr )
{}

/*
==================
CScriptSyntaxNode_Base::~CScriptSyntaxNode_Base
==================
*/
CScriptSyntaxNode_Base::~CScriptSyntaxNode_Base()
{}


/*
==================
CScriptSyntaxNode_Nop::Print
==================
*/
void CScriptSyntaxNode_Nop::Print( uint32 InPrintLevel /* = 0 */ )
{
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s Nop\n" ), this, GetContext().line, trailingSpace.c_str());
}

/*
==================
CScriptSyntaxNode_Nop::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Nop::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For OP_Nop operator we generate empty code
	CScriptCodeBlock*		codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "Nop" ) );
	codeBlock->GetBytecode().push_back( OP_Nop );
	return codeBlock;
}


/*
==================
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem
==================
*/
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InListRoot /* = nullptr */ )
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
CScriptSyntaxNode_ListItem::Print
==================
*/
void CScriptSyntaxNode_ListItem::Print( uint32 InPrintLevel /* = 0 */ )
{
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );

	Logf( TEXT( "%p(%4i):%s ListItem (Count: %i)\n" ), this, GetContext().line, trailingSpace.c_str(), nodes.size() );
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		nodes[index]->Print( InPrintLevel + 2 );
	}
}

/*
==================
CScriptSyntaxNode_ListItem::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_ListItem::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For list node we generate code for each item and glue together
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "ListItem" ) );
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		CScriptCodeBlock*		subCodeBlock = nodes[index]->GenerateCode( InCurrentContext );
		CScriptCodeBlock::Glue( codeBlock, subCodeBlock );
	}

	return codeBlock;
}

/*
==================
CScriptSyntaxNode_ListItem::MergeNode
==================
*/
void CScriptSyntaxNode_ListItem::MergeNode( CScriptSyntaxNode_Base* InNode )
{
	// If node is valid we add it to the list
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
CScriptSyntaxNode_Ident::Print
==================
*/
void CScriptSyntaxNode_Ident::Print( uint32 InPrintLevel /* = 0 */ )
{
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s Ident (Name: %s)\n" ), this, GetContext().line, trailingSpace.c_str(), ANSI_TO_TCHAR( name.c_str() ) );
}

/*
==================
CScriptSyntaxNode_Ident::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Ident::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	bool						bHasError = false;
	CScriptSyntaxNode_Base*		parentNode = GetParentNode();
	CScriptCodeBlock*			codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "Ident" ) );
	CMemoryWriter				codeWriter( codeBlock->GetBytecode() );
	if ( parentNode )
	{
		switch ( parentNode->GetType() )
		{
			// Generate code block for function call
		case SSNT_FuncCall:
		{
			CClass*			theClass = InCurrentContext->GetCreatedClass();
			Assert( theClass );
			CFunction*		function = theClass->FindFunction( ANSI_TO_TCHAR( name.c_str() ) );
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
}

/*
==================
CScriptSyntaxNode_FuncCall::Print
==================
*/
void CScriptSyntaxNode_FuncCall::Print( uint32 InPrintLevel /* = 0 */ )
{
	// Show node info
	std::wstring	trailingSpace;
	GetTrailingSpace( InPrintLevel, trailingSpace );

	Logf( TEXT( "%p(%4i):%s FuncCall\n" ), this, GetContext().line, trailingSpace.c_str() );
	if ( nameNode )
	{
		nameNode->Print( InPrintLevel + 2 );
	}
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
	CScriptCodeBlock*					codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "FuncCall" ) );
	codeBlock->GetBytecode().push_back( OP_Call );

	// Generate code for function name and glue it	
	CScriptCodeBlock*					codeBlockFuncIdent = nameNode->GenerateCode( InCurrentContext );
	CScriptCodeBlock::Glue( codeBlock, codeBlockFuncIdent );

	// We are done
	return codeBlock;
}