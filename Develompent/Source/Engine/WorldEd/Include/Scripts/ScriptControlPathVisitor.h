/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCONTROLPATHVISITOR_H
#define SCRIPTCONTROLPATHVISITOR_H

#include "Scripts/ScriptSyntaxNode.h"
#include "Scripts/ScriptSyntaxVisitor.h"

/**
 * @ingroup WorldEd
 * @brief Checker of control path in script code
 */
class CScriptControlPathVisitor : public CScriptSyntaxVisitor
{
public:
	/**
	 * @brief Control path node
	 */
	struct ControlPathNode
	{
		/**
		 * @brief Constructor
		 * @param InSyntaxNode		Node for which corresponds the control path node
		 * @param InPreviousNode	Previous control path node
		 */
		ControlPathNode( CScriptSyntaxNode_Base* InSyntaxNode, ControlPathNode* InPreviousNode = nullptr );

		/**
		 * @brief Destructor
		 */
		~ControlPathNode();

		/**
		 * @brief Check control paths
		 * @param InIsRequiredReturnValue		Is required return value
		 * @return Return TRUE if control paths is valid, otherwise FALSE
		 */
		bool CheckControlPaths( bool InIsRequiredReturnValue );

		/**
		 * @brief Get the end node
		 * @return Return the end node
		 */
		FORCEINLINE ControlPathNode* GetEndNode()
		{
			ControlPathNode*	endControlPathNode = this;
			while ( endControlPathNode->nextControlPathNode )
			{
				endControlPathNode = endControlPathNode->nextControlPathNode;
			}
			return endControlPathNode;
		}

		CScriptSyntaxNode_Base*		syntaxNode;					/**< Node for which corresponds the control path node */
		ControlPathNode*			nextControlPathNode;		/**< Next control path node */
		ControlPathNode*			previousControlPathNode;	/**< Previous control path node */
	};

	/**
	 * @brief Constructor
	 */
	CScriptControlPathVisitor();

	/**
	 * @brief Destructor
	 */
	~CScriptControlPathVisitor();

	/**
	 * @brief Get control path which was builded by CScriptSyntaxNode_Base::AcceptVisitor
	 * @return Return a control path root node. If isn't builded returns NULL
	 */
	FORCEINLINE ControlPathNode* GetRootPathNode() const
	{
		return rootPathNode;
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

private:
	/**
	 * @brief Creates a new control path for syntax node and update previousControlPathNode
	 * @param InNode			Node for which corresponds the control path node
	 * @param InPreviousNode	Previous control path node
	 */
	FORCEINLINE void MapControlPath( CScriptSyntaxNode_Base* InSyntaxNode, ControlPathNode* InPreviousNode = nullptr )
	{
		rootPathNode			= new ControlPathNode( InSyntaxNode, previousControlPathNode );
		previousControlPathNode = rootPathNode;
	}

	ControlPathNode*	rootPathNode;				/**< Root path node */
	ControlPathNode*	previousControlPathNode;	/**< Previous control path node */
};

#endif // !SCRIPTCONTROLPATHVISITOR_H