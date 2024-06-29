#include "Logger/LoggerMacros.h"
#include "System/MemoryArchive.h"
#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptSyntaxVisitor.h"

/*
==================
CScriptSyntaxNode_Base::CScriptSyntaxNode_Base
==================
*/
CScriptSyntaxNode_Base::CScriptSyntaxNode_Base( EScriptSyntaxNodeType InNodeType, const ScriptFileContext& InContext )
	: nodeType( InNodeType )
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
CScriptSyntaxNode_Nop::AcceptVisitor
==================
*/
void CScriptSyntaxNode_Nop::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_Nop( this );
}


/*
==================
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem
==================
*/
CScriptSyntaxNode_ListItem::CScriptSyntaxNode_ListItem( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InCodeRoot /* = nullptr */ )
	: CScriptSyntaxNode_Base( SSNT_ListItem, InContext )
{
	MergeNode( InCodeRoot );
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
CScriptSyntaxNode_ListItem::AcceptVisitor
==================
*/
void CScriptSyntaxNode_ListItem::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_ListItem( this );
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
		if ( InNode->GetNodeType() == SSNT_ListItem )
		{
			CScriptSyntaxNode_ListItem*			listNode = ( CScriptSyntaxNode_ListItem* )InNode;
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
CScriptSyntaxNode_Ident::AcceptVisitor
==================
*/
void CScriptSyntaxNode_Ident::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_Ident( this );
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
CScriptSyntaxNode_FuncCall::AcceptVisitor
==================
*/
void CScriptSyntaxNode_FuncCall::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_FuncCall( this );
}


/*
==================
CScriptSyntaxNode_Return::AcceptVisitor
==================
*/
void CScriptSyntaxNode_Return::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_Return( this );
}


/*
==================
CScriptSyntaxNode_IntConst::AcceptVisitor
==================
*/
void CScriptSyntaxNode_IntConst::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_IntConst( this );
}

/*
==================
CScriptSyntaxNode_FloatConst::AcceptVisitor
==================
*/
void CScriptSyntaxNode_FloatConst::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_FloatConst( this );
}