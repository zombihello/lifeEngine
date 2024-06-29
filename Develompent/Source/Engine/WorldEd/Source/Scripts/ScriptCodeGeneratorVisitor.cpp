#include "System/MemoryArchive.h"
#include "Scripts/ScriptCodeGeneratorVisitor.h"
#include "Reflection/FieldIterator.h"

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
CScriptCodeGeneratorVisitor::VisitSyntaxNode_ListItem
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_ListItem( class CScriptSyntaxNode_ListItem* InNode )
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
	
	// Generate code block for function call
	if ( parentSyntaxNode && parentSyntaxNode->GetNodeType() == SSNT_FuncCall )
	{
		CFunction*	function = InNode->GetCFunction();
		Assert( function );
		codeWriter << ( uptrint )function;
		
		// For function we are done
		return;
	}

	// Otherwise it is property and generate code for it
	CProperty*		property = InNode->GetCProperty();
	Assert( property );

	// Is this property in the function
	if ( property->GetOuter() == functionStub.GetCreatedFunction() )
	{
		codeWriter << ( byte )OP_LocalVariable;
	}
	// Is this property in the class
	else if ( property->GetOuter() == classStub.GetCreatedClass() )
	{
		codeWriter << ( byte )OP_ObjectVariable;
	}
	// Otherwise it is unknown scope and here we never must be
	else
	{
		AssertMsg( false, TEXT( "Unknown scope of property '%s'" ), property->GetFullName().c_str() );
	}

	codeWriter << ( uptrint )property;
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_FuncCall
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )
{
	// Generate code for function name
	CScriptSyntaxNode_Base*		syntaxNameNode		= InNode->GetNameNode();
	syntaxNameNode->AcceptVisitor( *this );
	Assert( codeBlocks );
	CScriptCodeBlock*			codeBlockFuncIdent	= codeBlocks;

	// Generate code for mark of end function parameters
	CScriptCodeBlock*			codeBlockEndFuncParams = new CScriptCodeBlock( InNode->GetContext(), TEXT( "EndFuncParams" ) );
	codeBlockEndFuncParams->GetBytecode().push_back( OP_EndFunctionParms );

	// Generate code for function parameters
	CScriptSyntaxNode_Base*		syntaxParamsNode	= InNode->GetParametersNode();
	CScriptCodeBlock*			codeBlockFuncParams = nullptr;

	// If we have parameters we generate the one's code and glue it with EndFuncParams block
	if ( syntaxParamsNode )
	{
		syntaxParamsNode->AcceptVisitor( *this );
		Assert( codeBlocks );
		codeBlockFuncParams = codeBlocks;
		CScriptCodeBlock::Glue( codeBlockFuncParams, codeBlockEndFuncParams );
	}
	// Otherwise we have only EndFuncParams block
	else
	{
		codeBlockFuncParams = codeBlockEndFuncParams;
	}

	// Generate code for function call and glue together all blocks 
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "FuncCall" ) );
	codeBlocks->GetBytecode().push_back( OP_Call );
	CScriptCodeBlock::Glue( codeBlocks, codeBlockFuncIdent );
	CScriptCodeBlock::Glue( codeBlocks, codeBlockFuncParams );
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

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_IntConst
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_IntConst( class CScriptSyntaxNode_IntConst* InNode )
{
	// Generate code with integer const
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "IntConst" ) );
	CMemoryWriter		codeWriter( codeBlocks->GetBytecode() );
	codeWriter << ( byte )OP_IntConst;
	codeWriter << InNode->GetValue();
}

/*
==================
CScriptCodeGeneratorVisitor::VisitSyntaxNode_FloatConst
==================
*/
void CScriptCodeGeneratorVisitor::VisitSyntaxNode_FloatConst( class CScriptSyntaxNode_FloatConst* InNode )
{
	// Generate code with integer const
	codeBlocks = new CScriptCodeBlock( InNode->GetContext(), TEXT( "FloatConst" ) );
	CMemoryWriter		codeWriter( codeBlocks->GetBytecode() );
	codeWriter << ( byte )OP_FloatConst;
	codeWriter << InNode->GetValue();
}