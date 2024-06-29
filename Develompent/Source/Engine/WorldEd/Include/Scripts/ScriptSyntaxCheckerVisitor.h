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
	 * @brief Visit syntax node 'list item'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_ListItem( class CScriptSyntaxNode_ListItem* InNode ) override;

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

	/**
	 * @brief Visit syntax node 'integer constant'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_IntConst( class CScriptSyntaxNode_IntConst* InNode ) override;

	/**
	 * @brief Visit syntax node 'float constant'
	 * @param InNode	Node to visit
	 */
	virtual void VisitSyntaxNode_FloatConst( class CScriptSyntaxNode_FloatConst* InNode ) override;

private:
	/**
	 * @brief Expression value type
	 */
	enum EExpressionValueType
	{
		EVT_Void,		/**< Void */
		EVT_Int,		/**< Integer */
		EVT_Float		/**< Float */

		//
		// Add here your a new expression value type
		//
	};

	/**
	 * @brief Last visited node data
	 */
	struct LastVisitedNodeData
	{
		/**
		 * @brief Constructor
		 */
		LastVisitedNodeData()
			: expressionValueType( EVT_Void )
		{}

		EExpressionValueType	expressionValueType;	/**< Expression value type */
	};

	/**
	 * @brief Check expression value type
	 * @param InExpressionValueType		Expression value type to check
	 * @param InProperty				Expected type
	 * @return Return TRUE if expression value type is valid, otherwise FALSE
	 */
	FORCEINLINE bool CheckExpressionValueType( EExpressionValueType InExpressionValueType, CProperty* InProperty ) const
	{
		return ConvertPropertyToExpressionValueType( InProperty ) == InExpressionValueType;
	}

	/**
	 * @brief Find property in scopes
	 * @param InName	Property name
	 * @return Return found property, if not found returns NULL
	 */
	CProperty* FindPropertyInScopes( const CName& InName ) const;

	/**
	 * @brief Convert property type into expression value type
	 * @param InProperty		Property to convert
	 * @return Return convert property type into expression value type
	 */
	EExpressionValueType ConvertPropertyToExpressionValueType( CProperty* InProperty ) const;

	bool						bHasError;				/**< There were syntax errors */
	const CScriptClassStub&		classStub;				/**< Class stub where functionStub is */
	const CScriptFunctionStub&	functionStub;			/**< Function stub where checkable code is */
	LastVisitedNodeData			lastVisitedNodeData;	/**< Data of last visited node */
};

#endif // !SCRIPTSYNTAXCHECKERVISITOR_H