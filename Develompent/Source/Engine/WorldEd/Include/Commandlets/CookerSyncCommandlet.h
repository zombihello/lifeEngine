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
class CCookerSyncCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS( CCookerSyncCommandlet, CBaseCommandlet, CLASS_Transient, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;

	/**
	 * Add content entries from file system
	 * 
	 * @pram InRootDir Root dir for start collection contents in table
	 */
	void AddContentEntries( const std::wstring& InRootDir );
};

#endif // !IMPORTTEXTURECOMMANDLET_H