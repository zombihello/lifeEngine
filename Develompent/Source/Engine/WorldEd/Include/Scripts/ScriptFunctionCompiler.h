/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTFUNCTIONCOMPILER_H
#define SCRIPTFUNCTIONCOMPILER_H

#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Function compiler
 */
class CScriptFunctionCompiler
{
public:
	/**
	 * @brief Constructor
	 * @param InStubs		Script stubs
	 * @param InIsShowDump	Is need show dump of syntax tree and code blocks
	 */
	CScriptFunctionCompiler( const CScriptSystemStub& InStubs, bool InIsShowDump = false );

	/**
	 * @brief Compile function
	 * 
	 * @param InClassStub			Class stub where InOutFunctionStub is
	 * @param InOutFunctionStub		Function stub to compile
	 * @return Return TRUE if function has been successfully compiled, otherwise FALSE
	 */
	bool Compile( const CScriptClassStub& InClassStub, CScriptFunctionStub& InOutFunctionStub );

private:
	bool							bShowDump;	/**< Is need show dump of syntax tree and code blocks */
	const CScriptSystemStub&		stubs;		/**< Stubs */
};

#endif // !SCRIPTFUNCTIONCOMPILER_H