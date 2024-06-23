#include "Scripts/ScriptControlPathVisitor.h"

/*
==================
CScriptControlPathVisitor::ControlPathNode::ControlPathNode
==================
*/
CScriptControlPathVisitor::ControlPathNode::ControlPathNode( CScriptSyntaxNode_Base* InSyntaxNode, ControlPathNode* InPreviousNode /* = nullptr */ )
	: syntaxNode( InSyntaxNode )
	, nextControlPathNode( nullptr )
	, previousControlPathNode( nullptr )
{
	// Link a new node with the previous node
	if ( InPreviousNode )
	{
		previousControlPathNode = InPreviousNode->GetEndNode();
		Assert( !previousControlPathNode->nextControlPathNode );
		InPreviousNode->nextControlPathNode = this;
	}
}

/*
==================
CScriptControlPathVisitor::ControlPathNode::~ControlPathNode
==================
*/
CScriptControlPathVisitor::ControlPathNode::~ControlPathNode()
{
	if ( nextControlPathNode )
	{
		delete nextControlPathNode;
	}
}

/*
==================
CScriptControlPathVisitor::ControlPathNode::CheckControlPaths
==================
*/
bool CScriptControlPathVisitor::ControlPathNode::CheckControlPaths( bool InIsRequiredReturnValue )
{
	// We need to examine this in reverse order
	ControlPathNode*	currentPathNode		= GetEndNode();
	uint32				linesAfterReturn	= 0;

	do
	{
		if ( currentPathNode->syntaxNode->GetType() == SSNT_Return )
		{
			if ( linesAfterReturn > 0 )
			{
				Warnf( TEXT( "%s: Return statement prevents the execution of successive lines of script\n" ), currentPathNode->syntaxNode->GetContext().ToString().c_str() );
			}
			return true;
		}

		++linesAfterReturn;
		currentPathNode = currentPathNode->previousControlPathNode;
	}
	while ( currentPathNode );

	if ( InIsRequiredReturnValue )
	{
		Errorf( TEXT( "%s: Not all control paths return a value\n" ), currentPathNode->syntaxNode->GetContext().ToString().c_str() );
	}
	return !InIsRequiredReturnValue;
}


/*
==================
CScriptControlPathVisitor::CScriptControlPathVisitor
==================
*/
CScriptControlPathVisitor::CScriptControlPathVisitor()
	: rootPathNode( nullptr )
	, previousControlPathNode( nullptr )
{}

/*
==================
CScriptControlPathVisitor::~CScriptControlPathVisitor
==================
*/
CScriptControlPathVisitor::~CScriptControlPathVisitor()
{
	if ( rootPathNode )
	{
		delete rootPathNode;
	}
}

/*
==================
CScriptControlPathVisitor::VisitSyntaxNode_Nop
==================
*/
void CScriptControlPathVisitor::VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )
{
	// To check control paths this is node useless
}

/*
==================
CScriptControlPathVisitor::VisitSyntaxNode_Code
==================
*/
void CScriptControlPathVisitor::VisitSyntaxNode_Code( class CScriptSyntaxNode_Code* InNode )
{
	// To check control paths this is node useless, but the one have child nodes that we need to concern ourselves with
	ControlPathNode*								firstPathNode	= nullptr;
	const std::vector<CScriptSyntaxNode_Base*>&		children		= InNode->GetNodes();
	for ( uint32 index = 0, count = children.size(); index < count; ++index )
	{
		children[index]->AcceptVisitor( *this );
		if ( !firstPathNode )
		{
			firstPathNode = rootPathNode;
		}
	}

	rootPathNode = firstPathNode;
}

/*
==================
CScriptControlPathVisitor::VisitSyntaxNode_Ident
==================
*/
void CScriptControlPathVisitor::VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )
{
	MapControlPath( InNode, previousControlPathNode );
}

/*
==================
CScriptControlPathVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptControlPathVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	MapControlPath( InNode, previousControlPathNode );
}

/*
==================
CScriptControlPathVisitor::VisitSyntaxNode_Return
==================
*/
void CScriptControlPathVisitor::VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )
{
	MapControlPath( InNode, previousControlPathNode );
}