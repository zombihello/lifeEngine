#include "System/MemoryArchive.h"
#include "Scripts/ScriptCodeGeneratorVisitor.h"

/*
==================
CScriptCodeGeneratorVisitor::CScriptCodeGeneratorVisitor
==================
*/
CScriptCodeGeneratorVisitor::CScriptCodeGeneratorVisitor( const CScriptClassStub& InClassStub, const CScriptFunctionStub& InFunctionStub )
	: codeBlocks( nullptr )
	, classStub( InClassStub )
	, functionStub( InFunctionStub )
{}

/*
==================
CScriptCodeGeneratorVisitor::~CScriptCodeGeneratorVisitor
==================
*/
CScriptCodeGeneratorVisitor::~CScriptCodeGeneratorVisitor()
{
	if ( codeBlocks )
	{
		delete codeBlocks;
	}
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_Nop
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )
{
	// For nop we do nothing
	codeBlocks = nullptr;
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_Code
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_Code( class CScriptSyntaxNode_Code* InNode )
{
	// To code generate this is node useless, but the one have child nodes that we need to concern ourselves with
	const std::vector<CScriptSyntaxNode_Base*>&	children		= InNode->GetNodes();
	CScriptCodeBlock*							firstCodeBlock	= nullptr;
	for ( uint32 index = 0, count = children.size(); index < count; ++index )
	{
		// Generate code for children node, result will be in codeBlocks
		children[index]->AcceptVisitor( *this );

		// If children code block was generated and it isn't the first - glue it! 
		if ( codeBlocks && firstCodeBlock )
		{
			CScriptCodeBlock::Glue( firstCodeBlock, codeBlocks );
		}
		// Otherwise this is the first block and we remember one
		else if ( codeBlocks )
		{
			firstCodeBlock = codeBlocks;
		}
	}

	// Store result into codeBlocks
	codeBlocks = firstCodeBlock;

	// If codeBlocks isn't valid it is mean what in the node hasn't any code.
	// In this case we create code block with OP_Nop
	if ( !codeBlocks )
	{
		codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "Nop" ) );
		codeBlocks->GetBytecode().push_back( OP_Nop );
	}
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_Ident
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )
{
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "Ident" ) );
	CScriptSyntaxNode_Base*		parentSyntaxNode = InNode->GetParentNode();
	CMemoryWriter				codeWriter( codeBlocks->GetBytecode() );
	AssertMsg( parentSyntaxNode, TEXT( "Missing parent syntax node" ) );

	switch ( parentSyntaxNode->GetType() )
	{
		// Generate code block for function call
	case SSNT_FuncCall:
	{
		CClass*		theClass = classStub.GetCreatedClass();
		Assert( theClass );
		CFunction*	function = theClass->FindFunction( ANSI_TO_TCHAR( InNode->GetName().c_str() ) );
		Assert( function );
		codeWriter << ( uptrint )function;
		break;
	}

		// Unknown parent, it is error
	default:
		AssertMsg( false, TEXT( "Unknown parent node" ) );
		break;
	}
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	// Generate code for function name
	CScriptSyntaxNode_Base*		syntaxNameNode = InNode->GetNameNode();
	syntaxNameNode->AcceptVisitor( *this );
	CScriptCodeBlock*			codeBlockFuncIdent = codeBlocks;

	// Generate code for function call and glue it
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "FuncCall" ) );
	codeBlocks->GetBytecode().push_back( OP_Call );
	CScriptCodeBlock::Glue( codeBlocks, codeBlockFuncIdent );
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_Return
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )
{
	// Generate return code
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "Return" ) );
	codeBlocks->GetBytecode().push_back( OP_Return );
}