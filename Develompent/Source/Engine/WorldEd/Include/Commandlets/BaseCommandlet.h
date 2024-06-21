/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASECOMMANDLET_H
#define BASECOMMANDLET_H

#include <string>

#include "Misc/CommandLine.h"
#include "Reflection/Object.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * Class of base commandlet
 */
class CBaseCommandlet : public CObject
{
	DECLARE_CLASS_NO_CTOR( CBaseCommandlet, CObject, CLASS_Intrinsic | CLASS_Transient | CLASS_Abstract, 0, TEXT("WorldEd") )

public:
	/**
	 * Main method of execute commandlet
	 * 
	 * @param InCommandLine		Command line
	 * @return Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 */
	virtual bool Main( const CCommandLine& InCommandLine ) PURE_VIRTUAL( CBaseCommandlet::Main, return false; );

	/**
	 * @brief Execute commandlet
	 *
	 * @param InCommandLine		Command line (-commandlet <CommandletName> <OtherArgs>)
	 * @param OutResultCommand	Return TRUE if commandlet executed is seccussed, otherwise will return FALSE
	 * @return Return TRUE if commandlet is executed, otherwise return FALSE
	 */
	static bool ExecCommandlet( const CCommandLine& InCommandLine, bool* OutResultCommand = nullptr );
};

#endif // !BASECOMMANDLET_H
