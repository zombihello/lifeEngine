/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCOMPILERCOMMANDLET_H
#define SCRIPTCOMPILERCOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"
#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Commandlet for compile LUA scripts
 */
class CScriptCompilerCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS_NO_CTOR( CScriptCompilerCommandlet, CBaseCommandlet, CLASS_Intrinsic | CLASS_Transient, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * @brief Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is successful, otherwise return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;

private:
	/**
	 * @brief Parse all scripts in directory
	 *
	 * @param InScriptDir		Path to script directory
	 * @param InOutStubs		Output script system stubs
	 * @param InRootDir			Path to root script directory. Need for make relative path to script file for logs
	 * @return Return TRUE if all scripts has been successfully parsed, otherwise FALSE
	 */
	bool ParseScripts( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs, const std::wstring& InRootDir );

	/**
	 * @brief Compile all scripts in the script system stubs
	 * @param InStubs		Script system stubs to compile
	 * @return Return TRUE if all scripts has been successfully compiled, otherwise FALSE
	 */
	bool CompileScripts( CScriptSystemStub& InStubs );
};

#endif // !SCRIPTCOMPILERCOMMANDLET_H