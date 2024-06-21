/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COOKPACKAGESCOMMANDLET_H
#define COOKPACKAGESCOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"

 /**
  * @ingroup WorldEd
  * @brief Commandlet for cooking packages
  */
class CCookPackagesCommandlet : public CBaseCommandlet
{
	DECLARE_CLASS_INTRINSIC( CCookPackagesCommandlet, CBaseCommandlet, CLASS_Intrinsic | CLASS_Transient, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * @brief Main method of execute commandlet
	 *
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) override;
};

#endif // !COOKPACKAGESCOMMANDLET_H