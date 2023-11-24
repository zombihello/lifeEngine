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
 * @brief Commandlet for compile scripts
 */
class CScriptCompilerCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS( CScriptCompilerCommandlet, CBaseCommandlet, 0, 0 )

public:
	/**
	 * @brief Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;

private:
	/**
	 * @brief Parse all scripts in directory
	 * 
	 * @param InScriptDir		Path to script directory
	 * @param InOutStubs		Output script system stubs
	 * @return Return TRUE if all scripts has been successfully parsed, otherwise returns FALSE
	 */
	bool ParseScripts( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs );

	/**
	 * @brief Generate C++ headers for native classes
	 * 
	 * @param InStubs			Script system stubs
	 * @return Return TRUE if any headers has been saved, otherwise returns FALSE
	 */
	bool GenerateNativeHeaders( CScriptSystemStub& InStubs );

	/**
	 * @brief Compile script functions
	 * 
	 * @param InStubs	Script system stubs
	 * @return Return TRUE if all script functions has been successfully compiled, otherwise returns FALSE
	 */
	bool CompileScripts( CScriptSystemStub& InStubs );

	/**
	 * @brief Save scripts to package
	 * 
	 * @param InStubs			Script system stubs
	 * @param InOutputDir		Filename to save
	 * @return Return TRUE if all script stubs has been successfully saved, otherwise returns FALSE
	 */
	bool SaveScripts( const std::wstring& InFilename, CScriptSystemStub& InStubs );
};

#endif // !SCRIPTCOMPILERCOMMANDLET_H