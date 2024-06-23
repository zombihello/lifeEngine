/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTSYNTAXCHECKERVISITOR_H
#define SCRIPTSYNTAXCHECKERVISITOR_H

#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptSyntaxVisitor.h"
#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Determine and check types of syntax nodes, handles casting
 */
class CScriptSyntaxCheckerVisitor : public CScriptSyntaxVisitor
{
public:
	/**
	 * @brief Constructor
	 * @param InClassStub			Class stub where InFunctionStub is
	 * @param InFunctionStub		Function stub where checkable code is
	 */
	CScriptSyntaxCheckerVisitor( const CScriptClassStub& InClassStub, const CScriptFunctionStub& InFunctionStub );

	/**
	 * @brief Has error
	 * @return Return TRUE if while moving on syntax tree by CScriptSyntaxNode_Base::AcceptVisitor were syntax error, otherwise FALSE
	 */
	FORCEINLINE bool HasError() const
	{
		return bHasError;
	}

protected:
	/**
	 * @brief Visit syntax node 'nop'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Nop( class CScriptSyntaxNode_Nop* InNode ) override;

	/**
	 * @brief Visit syntax node 'code'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Code( class CScriptSyntaxNode_Code* InNode ) override;

	/**
	 * @brief Visit syntax node 'ident'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Ident( class CScriptSyntaxNode_Ident* InNode ) override;

	/**
	 * @brief Visit syntax node 'function call'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_FuncCall( class CScriptSyntaxNode_FuncCall* InNode ) override;

	/**
	 * @brief Visit syntax node 'return'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_Return( class CScriptSyntaxNode_Return* InNode ) override;

private:
	bool						bHasError;		/**< There were syntax errors */
	const CScriptClassStub&		classStub;		/**< Class stub where functionStub is */
	const CScriptFunctionStub&	functionStub;	/**< Function stub where checkable code is */
};

#endif // !SCRIPTSYNTAXCHECKERVISITOR_H