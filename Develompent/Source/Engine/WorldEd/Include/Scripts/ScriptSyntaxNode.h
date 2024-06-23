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
	SSNT_Code,			/**< Code */
	SSNT_Ident,			/**< Identifier */
	SSNT_FuncCall,		/**< Function call */
	SSNT_Return			/**< Return */

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
	 * @param InType		Syntax node type
	 * @param InContext		Related place in the source code
	 */
	CScriptSyntaxNode_Base( EScriptSyntaxNodeType InType, const ScriptFileContext& InContext );

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
	FORCEINLINE EScriptSyntaxNodeType GetType() const
	{
		return type;
	}

private:
	EScriptSyntaxNodeType		type;		/**< Syntax node type */
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
 * @brief Syntax node of code
 */
class CScriptSyntaxNode_Code : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext		Related place in the source code
	 * @param InCodeRoot	Root of the code
	 */
	CScriptSyntaxNode_Code( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InCodeRoot = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptSyntaxNode_Code();

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
	 */
	CScriptSyntaxNode_FuncCall( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InFunctionName )
		: CScriptSyntaxNode_Base( SSNT_FuncCall, InContext )
		, nameNode( nullptr )
	{
		SetNameNode( InFunctionName );
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

private:
	CScriptSyntaxNode_Base*		nameNode;		/**< Function name */
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

#endif // !SCRIPTSYNTAXNODE_H