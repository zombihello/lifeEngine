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
#include "Scripts/ScriptCompilerEnvironment.h"

/**
 * @ingroup WorldEd
 * @brief Function compiler
 */
class CScriptFunctionCompiler
{
public:
	/**
	 * @brief Constructor
	 * @param InEnvironment		Script compiler environment
	 * @param InVerbose			Is need to print verbose logs
	 */
	CScriptFunctionCompiler( const CScriptCompilerEnvironment& InEnvironment, bool InVerbose = false );

	/**
	 * @brief Compile function
	 * 
	 * @param InClassStub			Class stub where InOutFunctionStub is
	 * @param InOutFunctionStub		Function stub to compile
	 * @return Return TRUE if function has been successfully compiled, otherwise returns FALSE
	 */
	bool Compile( const CScriptClassStub& InClassStub, CScriptFunctionStub& InOutFunctionStub );

private:
	bool									bVerbose;		/**< Is need to print verbose logs */
	const CScriptCompilerEnvironment*		environment;	/**< Compiler environment */
};

#endif // !SCRIPTFUNCTIONCOMPILER_H