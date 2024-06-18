/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCODEGENERATOR_H
#define SCRIPTCODEGENERATOR_H

#include "Scripts/ScriptFileContext.h"
#include "Scripts/ScriptSyntaxNode.h"

/**
 * @ingroup WorldEd
 * @brief Byte code generator for compiled script functions
 */
class CScriptCodeGenerator
{
public:
	/**
	 * @brief Generate bytecode from syntax tree
	 * 
	 * @param InClassStub		Class stub
	 * @param InSyntaxTree		Syntax tree
	 * @param InIsShowDump		Is need show dump of syntax tree and code blocks
	 */
	void GenerateCode( const CScriptClassStub& InClassStub, CScriptSyntaxNode_Base* InSyntaxTree, bool InIsShowDump = false );

	/**
	 * @brief Get generated bytecode
	 * @return Return generated bytecode. If it isn't was generated return empty array
	 */
	FORCEINLINE const std::vector<byte>& GetBytecode() const
	{
		return bytecode;
	}

private:
	std::vector<byte>		bytecode;		/**< Compiled bytecode */
};

#endif // !SCRIPTCODEGENERATOR_H