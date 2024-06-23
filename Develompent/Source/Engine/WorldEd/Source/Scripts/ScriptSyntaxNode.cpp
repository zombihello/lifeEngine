#include "Logger/LoggerMacros.h"
#include "System/MemoryArchive.h"
#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptSyntaxVisitor.h"

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
CScriptSyntaxNode_Nop::AcceptVisitor
==================
*/
void CScriptSyntaxNode_Nop::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_Nop( this );
}


/*
==================
CScriptSyntaxNode_Code::CScriptSyntaxNode_Code
==================
*/
CScriptSyntaxNode_Code::CScriptSyntaxNode_Code( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InCodeRoot /* = nullptr */ )
	: CScriptSyntaxNode_Base( SSNT_Code, InContext )
{
	MergeNode( InCodeRoot );
}

/*
==================
CScriptSyntaxNode_Code::~CScriptSyntaxNode_Code
==================
*/
CScriptSyntaxNode_Code::~CScriptSyntaxNode_Code()
{
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		delete nodes[index];
	}
}

/*
==================
CScriptSyntaxNode_Code::AcceptVisitor
==================
*/
void CScriptSyntaxNode_Code::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_Code( this );
}

/*
==================
CScriptSyntaxNode_Code::MergeNode
==================
*/
void CScriptSyntaxNode_Code::MergeNode( CScriptSyntaxNode_Base* InNode )
{
	// If node is valid we add it to the list
	if ( InNode )
	{
		if ( InNode->GetType() == SSNT_Code )
		{
			CScriptSyntaxNode_Code*			listNode = ( CScriptSyntaxNode_Code* )InNode;
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