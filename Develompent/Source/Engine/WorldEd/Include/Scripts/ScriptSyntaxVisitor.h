/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTSYNTAXVISITOR_H
#define SCRIPTSYNTAXVISITOR_H

#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Base class of visitor for syntax nodes
 */
class CScriptSyntaxVisitor
{
	friend class CScriptSyntaxNode_Nop;
	friend class CScriptSyntaxNode_ListItem;
	friend class CScriptSyntaxNode_Ident;
	friend class CScriptSyntaxNode_FuncCall;
	friend class CScriptSyntaxNode_Return;
	friend class CScriptSyntaxNode_IntConst;
	friend class CScriptSyntaxNode_FloatConst;

protected:
	/**
	 * @brief Visit syntax node 'nop'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode )							PURE_VIRTUAL( CScriptSyntaxVisitor::VisitSyntaxNode_Nop, );

	/**
	 * @brief Visit syntax node 'list item'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_ListItem( class CScriptSyntaxNode_ListItem* InNode )				PURE_VIRTUAL( CScriptSyntaxVisitor::VisitSyntaxNode_ListItem, );

	/**
	 * @brief Visit syntax node 'ident'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode )						PURE_VIRTUAL( CScriptSyntaxVisitor::CScriptSyntaxNode_Ident, );

	/**
	 * @brief Visit syntax node 'function call'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode )				PURE_VIRTUAL( CScriptSyntaxVisitor::CScriptSyntaxNode_FuncCall, );

	/**
	 * @brief Visit syntax node 'return'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode )					PURE_VIRTUAL( CScriptSyntaxVisitor::CScriptSyntaxNode_Return, );

	/**
	 * @brief Visit syntax node 'integer constant'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_IntConst( class CScriptSyntaxNode_IntConst* InNode )				PURE_VIRTUAL( CScriptSyntaxVisitor::CScriptSyntaxNode_IntConst, );

	/**
	 * @brief Visit syntax node 'float constant'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_FloatConst( class CScriptSyntaxNode_FloatConst* InNode )			PURE_VIRTUAL( CScriptSyntaxVisitor::CScriptSyntaxNode_FloatConst, );
};

#endif // !SCRIPTSYNTAXVISITOR_H