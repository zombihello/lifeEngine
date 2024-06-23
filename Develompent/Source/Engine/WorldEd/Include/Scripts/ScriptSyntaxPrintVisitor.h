/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTSYNTAXPRINTVISITOR_H
#define SCRIPTSYNTAXPRINTVISITOR_H

#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptSyntaxVisitor.h"

/**
 * @ingroup WorldEd
 * @brief Print syntax nodes to log
 */
class CScriptSyntaxPrintVisitor : public CScriptSyntaxVisitor
{
public:
	/**
	 * @brief Constructor
	 */
	CScriptSyntaxPrintVisitor();

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
	/**
	 * @brief Get trailing space for print node
	 *
	 * @param InPrintLevel		Print level
	 * @param OutString			Output string with trailing spaces
	 */
	FORCEINLINE void GetTrailingSpace( uint32 InPrintLevel, std::wstring& OutString )
	{
		for ( uint32 index = 0; index < InPrintLevel; ++index )
		{
			OutString += TEXT( " " );
		}
	}

	uint32		printLevel;		/**< Print level */
};

#endif // !SCRIPTSYNTAXPRINTVISITOR_H