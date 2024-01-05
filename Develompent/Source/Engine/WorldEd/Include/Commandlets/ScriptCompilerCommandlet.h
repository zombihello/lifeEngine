/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTCOMPILERCOMMANDLET_H
#define SCRIPTCOMPILERCOMMANDLET_H

#include <vector>

#include "Commandlets/BaseCommandlet.h"
#include "Scripts/ScriptFieldStubs.h"

/**
 * @ingroup WorldEd
 * @brief Commandlet for compile scripts
 */
class CScriptCompilerCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS( CScriptCompilerCommandlet, CBaseCommandlet, 0, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * @brief Constructor
	 */
	CScriptCompilerCommandlet();

	/**
	 * @brief Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;

private:
	/**
	 * @brief Parse all script classes in directory
	 * 
	 * @param InScriptDir		Path to script directory
	 * @param InOutStubs		Output script system stubs
	 * @return Return TRUE if all script classes has been successfully parsed, otherwise returns FALSE
	 */
	bool ParseScriptClasses( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs );

	/**
	 * @brief Compile script classes
	 * 
	 * @param InStubs	Script system stubs
	 * @return Return TRUE if all script classes has been successfully compiled, otherwise returns FALSE
	 */
	bool CompileScriptClasses( CScriptSystemStub& InStubs );

	const std::wstring			contentDir;		/**< Path to game content directory */
	const std::wstring			scriptsDir;		/**< Path to game scripts directory */
	const std::wstring			outputFile;		/**< Path to output file with script classes */
};

#endif // !SCRIPTCOMPILERCOMMANDLET_H