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
CScriptSyntaxNode_Code::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Code::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For list node we generate code for each item and glue together
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "Code" ) );
	for ( uint32 index = 0, count = nodes.size(); index < count; ++index )
	{
		CScriptCodeBlock*		subCodeBlock = nodes[index]->GenerateCode( InCurrentContext );
		CScriptCodeBlock::Glue( codeBlock, subCodeBlock );
	}

	return codeBlock;
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
CScriptSyntaxNode_FuncCall::AcceptVisitor
==================
*/
void CScriptSyntaxNode_FuncCall::AcceptVisitor( CScriptSyntaxVisitor& InVisitor )
{
	InVisitor.VisitSyntaxNode_FuncCall( this );
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
CScriptSyntaxNode_Return::GenerateCode
==================
*/
CScriptCodeBlock* CScriptSyntaxNode_Return::GenerateCode( CScriptClassStub*& InCurrentContext )
{
	// For OP_Nop operator we generate empty code
	CScriptCodeBlock*	codeBlock = new CScriptCodeBlock( GetContext(), TEXT( "Return" ) );
	codeBlock->GetBytecode().push_back( OP_Return );
	return codeBlock;
}