/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTSYNTAXNODE_H
#define SCRIPTSYNTAXNODE_H

#include "Scripts/ScriptFileContext.h"
#include "Scripts/ScriptCodeBlock.h"
#include "Scripts/ScriptFieldStubs.h"
#include "Scripts/ScriptSyntaxVisitor.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration type syntax nodes
 */
enum EScriptSyntaxNodeType
{
	SSNT_Nop,			/**< No operation */
	SSNT_ListItem,		/**< List of items */
	SSNT_Ident,			/**< Identifier */
	SSNT_FuncCall,		/**< Function call */
	SSNT_Return,		/**< Return */
	SSNT_IntConst		/**< Integer constant */

	//
	// Add here your a new syntax node type
	//
};

/**
 * @ingroup WorldEd
 * @brief Base syntax node of script function code
 */
class CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InNodeType	Syntax node type
	 * @param InContext		Related place in the source code
	 */
	CScriptSyntaxNode_Base( EScriptSyntaxNodeType InNodeType, const ScriptFileContext& InContext );

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptSyntaxNode_Base();
	
	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) = 0;

	/**
	 * @brief Set parent node
	 * @param InParentNode		A new parent node
	 */
	FORCEINLINE void SetParentNode( CScriptSyntaxNode_Base* InParentNode )
	{
		parentNode = InParentNode;
	}

	/**
	 * @brief Get related place in the source code
	 * @return Return related place in the source code
	 */
	FORCEINLINE ScriptFileContext& GetContext()
	{
		return context;
	}

	/**
	 * @brief Get related place in the source code
	 * @return Return related place in the source code
	 */
	FORCEINLINE const ScriptFileContext& GetContext() const
	{
		return context;
	}

	/**
	 * @brief Get parent node
	 * @return Return parent node. If this root returns NULL
	 */
	FORCEINLINE CScriptSyntaxNode_Base* GetParentNode() const
	{
		return parentNode;
	}

	/**
	 * @brief Get node type
	 * @return Return node type
	 */
	FORCEINLINE EScriptSyntaxNodeType GetNodeType() const
	{
		return nodeType;
	}

private:
	EScriptSyntaxNodeType		nodeType;	/**< Syntax node type */
	ScriptFileContext			context;	/**< Related place in the source code */
	CScriptSyntaxNode_Base*		parentNode;	/**< Parent syntax node */
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of no operation
 */
class CScriptSyntaxNode_Nop : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 * @param InContext		Related place in the source code
	 */
	CScriptSyntaxNode_Nop( const ScriptFileContext& InContext )
		: CScriptSyntaxNode_Base( SSNT_Nop, InContext )
	{}

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;
};

/**
 * @ingroup WorldEd
 * @brief Syntax node list of items
 */
class CScriptSyntaxNode_ListItem : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext		Related place in the source code
	 * @param InCodeRoot	Root of the code
	 */
	CScriptSyntaxNode_ListItem( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InCodeRoot = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptSyntaxNode_ListItem();

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;

	/**
	 * @brief Add node to code
	 * @param InNode	A new node
	 */
	FORCEINLINE void AddNode( CScriptSyntaxNode_Base* InNode )
	{
		Assert( InNode );
		MergeNode( InNode );
	}

	/**
	 * @brief Get code nodes
	 * @return Return code list
	 */
	FORCEINLINE const std::vector<CScriptSyntaxNode_Base*>& GetNodes() const
	{
		return nodes;
	}

	/**
	 * @brief Get number of nodes in the code
	 * @return Return number of nodes in the code
	 */
	FORCEINLINE uint32 GetNumNodes() const
	{
		return nodes.size();
	}

private:
	/**
	 * @brief Merge node
	 * @param InNode	Syntax node
	 */
	void MergeNode( CScriptSyntaxNode_Base* InNode );

	std::vector<CScriptSyntaxNode_Base*>	nodes;	/**< Nodes list */
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of identifier
 */
class CScriptSyntaxNode_Ident : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext		Related place in the source code
	 * @param InIdentName	Identifier name
	 */
	CScriptSyntaxNode_Ident( const ScriptFileContext& InContext, const std::string_view& InIdentName )
		: CScriptSyntaxNode_Base( SSNT_Ident, InContext )
		, name( InIdentName )
	{}

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;

	/**
	 * @brief Get identifier name
	 * @return Return identifier name
	 */
	FORCEINLINE const std::string& GetName() const
	{
		return name;
	}

private:
	std::string		name;	/**< Identifier name */
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of function call
 */
class CScriptSyntaxNode_FuncCall : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext			Related place in the source code
	 * @param InFunctionName	Function name
	 * @param InFunctionParams	Function parameters
	 */
	CScriptSyntaxNode_FuncCall( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InFunctionName, CScriptSyntaxNode_Base* InFunctionParams )
		: CScriptSyntaxNode_Base( SSNT_FuncCall, InContext )
		, nameNode( nullptr )
		, parametersNode( nullptr )
	{
		SetNameNode( InFunctionName );
		SetParametersNode( InFunctionParams );
	}

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptSyntaxNode_FuncCall();

	/**
	 * @brief Set function name node
	 * @param InFunctionName	A new function name node
	 */
	FORCEINLINE void SetNameNode( CScriptSyntaxNode_Base* InFunctionName )
	{
		if ( nameNode && nameNode->GetParentNode() == this )
		{
			delete nameNode;
			nameNode = nullptr;
		}

		nameNode = InFunctionName;
		if ( nameNode )
		{
			nameNode->SetParentNode( this );
		}
	}

	/**
	 * @brief Set function parameters node
	 * @param InFunctonParams	A new function parameters node
	 */
	FORCEINLINE void SetParametersNode( CScriptSyntaxNode_Base* InFunctonParams )
	{
		if ( parametersNode )
		{
			delete parametersNode;
			parametersNode = nullptr;
		}
		parametersNode = InFunctonParams;
	}

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;

	/**
	 * @brief Get function name
	 * @return Return function name
	 */
	FORCEINLINE CScriptSyntaxNode_Base* GetNameNode() const
	{
		return nameNode;
	}

	/**
	 * @brief Get function parameters
	 * @return Return function parameters
	 */
	FORCEINLINE CScriptSyntaxNode_Base* GetParametersNode() const
	{
		return parametersNode;
	}

private:
	CScriptSyntaxNode_Base*		nameNode;		/**< Function name */
	CScriptSyntaxNode_Base*		parametersNode;	/**< Function parameters */
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of return
 */
class CScriptSyntaxNode_Return : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 * @param InContext		Related place in the source code
	 */
	CScriptSyntaxNode_Return( const ScriptFileContext& InContext )
		: CScriptSyntaxNode_Base( SSNT_Return, InContext )
	{}

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of integer constant
 */
class CScriptSyntaxNode_IntConst : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InContext		Related place in the source code
	 * @param InValue		Integer value
	 */
	CScriptSyntaxNode_IntConst( const ScriptFileContext& InContext, uint32 InValue )
		: CScriptSyntaxNode_Base( SSNT_IntConst, InContext )
		, value( InValue )
	{}

	/**
	 * @brief Accept visitor
	 * @param InVisitor		Visitor
	 */
	virtual void AcceptVisitor( CScriptSyntaxVisitor& InVisitor ) override;

	/**
	 * @brief Set integer value
	 * @param InValue	A new value
	 */
	FORCEINLINE void SetValue( uint32 InValue )
	{
		value = InValue;
	}

	/**
	 * @brief Get integer value
	 * @return Return integer value
	 */
	FORCEINLINE uint32 GetValue() const
	{
		return value;
	}

private:
	uint32		value;		/**< Integer value */
};

#endif // !SCRIPTSYNTAXNODE_H