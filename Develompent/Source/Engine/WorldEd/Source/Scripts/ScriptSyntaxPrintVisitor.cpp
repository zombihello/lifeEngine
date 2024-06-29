#include "Scripts/ScriptSyntaxPrintVisitor.h"

/*
==================
CScriptSyntaxPrintVisitor::CScriptSyntaxPrintVisitor
==================
*/
CScriptSyntaxPrintVisitor::CScriptSyntaxPrintVisitor()
	: printLevel( 0 )
{}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_Nop
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s Nop\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str() );
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_ListItem
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_ListItem( class CScriptSyntaxNode_ListItem* InNode )
{
	std::wstring									trailingSpace;
	const std::vector<CScriptSyntaxNode_Base*>&		children = InNode->GetNodes();
	GetTrailingSpace( printLevel, trailingSpace );

	Logf( TEXT( "%p(%4i):%s ListItem (Count: %i)\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str(), children.size() );
	for ( uint32 index = 0, count = children.size(); index < count; ++index )
	{
		TGuardValue<uint32>		printLevelGuard( printLevel, printLevel + 2 );
		children[index]->AcceptVisitor( *this );
	}
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_Ident
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s Ident (Name: %s)\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str(), ANSI_TO_TCHAR( InNode->GetName().c_str() ) );
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	std::wstring				trailingSpace;
	CScriptSyntaxNode_Base*		syntaxNameNode = InNode->GetNameNode();
	CScriptSyntaxNode_Base*		syntaxParamsNode = InNode->GetParametersNode();
	GetTrailingSpace( printLevel, trailingSpace );

	Logf( TEXT( "%p(%4i):%s FuncCall\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str() );
	
	// Print function name
	if ( syntaxNameNode )
	{
		TGuardValue<uint32>		printLevelGuard( printLevel, printLevel + 2 );
		syntaxNameNode->AcceptVisitor( *this );
	}

	// Print function parameters
	if ( syntaxParamsNode )
	{
		TGuardValue<uint32>		printLevelGuard( printLevel, printLevel + 2 );
		syntaxParamsNode->AcceptVisitor( *this );
	}
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_Return
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s Return\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str() );
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_IntConst
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_IntConst( class CScriptSyntaxNode_IntConst* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s IntConst (Value: %i)\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str(), InNode->GetValue() );
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_FloatConst
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_FloatConst( class CScriptSyntaxNode_FloatConst* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s FloatConst (Value: %f)\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str(), InNode->GetValue() );
}

/*
==================
CScriptSyntaxPrintVisitor::VisitSyntaxNode_StringConst
==================
*/
void CScriptSyntaxPrintVisitor::VisitSyntaxNode_StringConst( class CScriptSyntaxNode_StringConst* InNode )
{
	std::wstring	trailingSpace;
	GetTrailingSpace( printLevel, trailingSpace );
	Logf( TEXT( "%p(%4i):%s StringConst (Value: %s)\n" ), InNode, InNode->GetContext().line, trailingSpace.c_str(), ANSI_TO_TCHAR( InNode->GetValue().c_str() ) );
}