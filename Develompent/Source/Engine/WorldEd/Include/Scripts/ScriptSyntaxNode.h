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

/**
 * @ingroup WorldEd
 * @brief Enumeration type syntax nodes
 */
enum EScriptSyntaxNodeType
{
	SSNT_Nop,			/**< No operation */
	SSNT_ListItem,		/**< List of other nodes */
	SSNT_Ident,			/**< Identifier */
	SSNT_FuncCall		/**< Function call */

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
	 * @brief Write syntax tree to log
	 * @param InPrintLevel		Print level
	 */
	virtual void Print( uint32 InPrintLevel = 0 ) = 0;

	/**
	 * @brief Generate code blocks from this syntax node
	 * @note You need to free allocated memory after use
	 *
	 * @param InCurrentContext		Current context. Function may change it
	 * @return Return generated code blocks
	 */
	virtual CScriptCodeBlock* GenerateCode( CScriptClassStub*& InCurrentContext ) = 0;

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

protected:
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
	 * @brief Write syntax tree to log
	 * @param InPrintLevel		Print level
	 */
	virtual void Print( uint32 InPrintLevel = 0 ) override;

	/**
	 * @brief Generate code blocks from this syntax node
	 * @note You need to free allocated memory after use
	 *
	 * @param InCurrentContext	Current context. Function may change it
	 * @return Return generated code blocks
	 */
	virtual CScriptCodeBlock* GenerateCode( CScriptClassStub*& InCurrentContext ) override;
};

/**
 * @ingroup WorldEd
 * @brief Syntax node of list items
 */
class CScriptSyntaxNode_ListItem : public CScriptSyntaxNode_Base
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InContext		Related place in the source code
	 * @param InListRoot	Root of the list
	 */
	CScriptSyntaxNode_ListItem( const ScriptFileContext& InContext, CScriptSyntaxNode_Base* InListRoot = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~CScriptSyntaxNode_ListItem();

	/**
	 * @brief Write syntax tree to log
	 * @param InPrintLevel		Print level
	 */
	virtual void Print( uint32 InPrintLevel = 0 ) override;

	/**
	 * @brief Generate code blocks from this syntax node
	 * @note You need to free allocated memory after use
	 *
	 * @param InCurrentContext	Current context. Function may change it
	 * @return Return generated code blocks
	 */
	virtual CScriptCodeBlock* GenerateCode( CScriptClassStub*& InCurrentContext ) override;

	/**
	 * @brief Add node to list
	 * @param InNode	A new node
	 */
	FORCEINLINE void AddNode( CScriptSyntaxNode_Base* InNode )
	{
		Assert( InNode );
		MergeNode( InNode );
	}

	/**
	 * @brief Get nodes list
	 * @return Return nodes list
	 */
	FORCEINLINE const std::vector<CScriptSyntaxNode_Base*>& GetNodes() const
	{
		return nodes;
	}

	/**
	 * @brief Get number of nodes in the list
	 * @return Return number of nodes in the list
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
	 * @brief Write syntax tree to log
	 * @param InPrintLevel		Print level
	 */
	virtual void Print( uint32 InPrintLevel = 0 ) override;

	/**
	 * @brief Generate code blocks from this syntax node
	 * @note You need to free allocated memory after use
	 *
	 * @param InCurrentContext	Current context. Function may change it
	 * @return Return generated code blocks
	 */
	virtual CScriptCodeBlock* GenerateCode( CScriptClassStub*& InCurrentContext ) override;

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
	 * @brief Write syntax tree to log
	 * @param InPrintLevel		Print level
	 */
	virtual void Print( uint32 InPrintLevel = 0 ) override;

	/**
	 * @brief Generate code blocks from this syntax node
	 * @note You need to free allocated memory after use
	 *
	 * @param InCurrentContext	Current context. Function may change it
	 * @return Return generated code blocks
	 */
	virtual CScriptCodeBlock* GenerateCode( CScriptClassStub*& InCurrentContext ) override;

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

#endif // !SCRIPTSYNTAXNODE_H