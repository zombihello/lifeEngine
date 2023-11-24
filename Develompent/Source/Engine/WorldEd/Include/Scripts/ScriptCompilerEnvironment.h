/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCOMPILERENVIRONMENT_H
#define SCRIPTCOMPILERENVIRONMENT_H

#include "Core.h"
#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Script compiler environment
 */
class CScriptCompilerEnvironment
{
public:
	/**
	 * @brief Constructor
	 * @param InStubs	Stubs
	 */
	CScriptCompilerEnvironment( CScriptSystemStub& InStubs );

private:
	CScriptSystemStub*	stubs;	/**< Stubs */
};

#endif // !SCRIPTCOMPILERENVIRONMENT_H