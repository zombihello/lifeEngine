/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COOKERSYNCCOMMANDLET_H
#define COOKERSYNCCOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"

/**
 * @ingroup WorldEd
 * Commandlet for generate table of contents
 */
class LCookerSyncCommandlet : public LBaseCommandlet
{
	DECLARE_CLASS( LCookerSyncCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommand ) override;

	/**
	 * Add content entries from file system
	 * 
	 * @pram InRootDir Root dir for start collection contents in table
	 */
	void AddContentEntries( const std::wstring& InRootDir );
};

#endif // !IMPORTTEXTURECOMMANDLET_H