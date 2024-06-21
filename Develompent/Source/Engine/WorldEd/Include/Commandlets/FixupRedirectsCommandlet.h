/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FIXUPREDIRECTSCOMMANDLET_H
#define FIXUPREDIRECTSCOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"

/**
 * @ingroup WorldEd
 * @brief Commandlet for fixup redirects in packages
 */
class CFixupRedirectsCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS_NO_CTOR( CFixupRedirectsCommandlet, CBaseCommandlet, CLASS_Intrinsic | CLASS_Transient, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * @brief Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is successful, otherwise return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;
};

#endif // !FIXUPREDIRECTSCOMMANDLET_H