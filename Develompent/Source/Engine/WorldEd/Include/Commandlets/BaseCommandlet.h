/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASECOMMANDLET_H
#define BASECOMMANDLET_H

#include <string>

#include "Misc/Object.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * Class of base commandlet
 */
class LBaseCommandlet : public LObject
{
	DECLARE_CLASS( LBaseCommandlet, LObject )

public:
	/**
	 * Destructor
	 */
	virtual ~LBaseCommandlet() {}

	/**
	 * Main method of execute commandlet
	 * 
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommands ) PURE_VIRTUAL( LBaseCommandlet::Main, return false; );
};

#endif // !BASECOMMANDLET_H