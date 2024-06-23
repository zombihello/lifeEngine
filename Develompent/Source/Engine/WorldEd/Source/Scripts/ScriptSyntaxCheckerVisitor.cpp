#include "Scripts/ScriptSyntaxCheckerVisitor.h"

/*
==================
CScriptSyntaxCheckerVisitor::CScriptSyntaxCheckerVisitor
==================
*/
CScriptSyntaxCheckerVisitor::CScriptSyntaxCheckerVisitor( const CScriptClassStub& InClassStub, const CScriptFunctionStub& InFunctionStub )
	: bHasError( false )
	, classStub( InClassStub )
	, functionStub( InFunctionStub )
{}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Nop
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )
{}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Code
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Code( class CScriptSyntaxNode_Code* InNode )
{
	// Check on syntax errors in children
	const std::vector<CScriptSyntaxNode_Base*>&		children = InNode->GetNodes();
	for ( uint32 index = 0, count = children.size(); index < count; ++index )
	{
		children[index]->AcceptVisitor( *this );
	}
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Ident
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )
{
	CScriptSyntaxNode_Base*		parentNode = InNode->GetParentNode();

	// Is this identifier its function call
	if ( parentNode && parentNode->GetType() == SSNT_FuncCall )
	{
		// Try find this function in the class
		CClass*		theClass = classStub.GetCreatedClass();
		CFunction*	function = theClass->FindFunction( ANSI_TO_TCHAR( InNode->GetName().c_str() ) );
		if ( !function )
		{
			Errorf( TEXT( "%s: Could not find function '%s'\n" ), InNode->GetContext().ToString().c_str(), ANSI_TO_TCHAR( InNode->GetName().c_str() ) );
			bHasError = true;
		}

		// For function call we are done
		return;
	}

	// Anything else we don't currently support
	Errorf( TEXT( "%s: Identifiers can currently only be used to call a function\n" ), InNode->GetContext().ToString().c_str() );
	bHasError = true;
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	// Check function identifier
	CScriptSyntaxNode_Base*		syntaxNameNode = InNode->GetNameNode();
	Assert( syntaxNameNode );
	syntaxNameNode->AcceptVisitor( *this );
}

/*
==================
CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Return
==================
*/
void CScriptSyntaxCheckerVisitor::VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )
{}