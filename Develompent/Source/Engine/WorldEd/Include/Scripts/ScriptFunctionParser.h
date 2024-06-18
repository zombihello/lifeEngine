/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFUNCTIONPARSER_H
#define SCRIPTFUNCTIONPARSER_H

#include "Scripts/ScriptFieldStubs.h"
#include "Scripts/ScriptSyntaxNode.h"

/**
 * @ingroup WorldEd
 * @brief Script function parser
 */
class CScriptFunctionParser
{
public:
	/**
	 * @brief Constructor
	 */
	CScriptFunctionParser();

	/**
	 * @brief Parse function code
	 *
	 * @param IFunctionStub		Function stub to parse
	 * @return Return the constructed syntax tree
	 */
	CScriptSyntaxNode_Base* ParseFunctionCode( CScriptFunctionStub* InFunctionStub );

	/**
	 * @brief Emit parsing error
	 *
	 * @param InContext		Script file context
	 * @param InMessage		Message
	 */
	void EmitError( const ScriptFileContext& InContext, const std::wstring& InMessage );

	/**
	 * @brief Set root syntax node for compiled function
	 * @param InNode	Syntax node
	 */
	void SetRootSyntaxNode( CScriptSyntaxNode_Base* InNode );

private:
	bool					bHasError;			/**< There were a parsing errors */
	CScriptSyntaxNode_Base*	rootSyntaxNode;		/**< Compiler root syntax node */
};

#endif // !SCRIPTFUNCTIONPARSER_H