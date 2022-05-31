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

#include "Misc/Object.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * Class of base commandlet
 */
class ÑBaseCommandlet : public CObject
{
	DECLARE_CLASS( ÑBaseCommandlet, CObject )

public:
	/**
	 * Destructor
	 */
	virtual ~ÑBaseCommandlet() {}

	/**
	 * Main method of execute commandlet
	 * 
	 * @param[in] InCommands Arguments of start commandlet
	 * @return Return true if commandlet executed is seccussed, else returning false
	 */
	virtual bool Main( const std::wstring& InCommands ) PURE_VIRTUAL( ÑBaseCommandlet::Main, return false; );

	/**
	 * @brief Execute commandlet
	 *
	 * @param InCommands Arguments of start commandlet
	 * @param InBaseCommandIndex Base index of commands in InCommands
	 * @param OutResultCommand Return true if commandlet executed is seccussed, else returning false
	 * @return Return true if commandlet is executed, else returning false
	 */
	static bool ExecCommandlet( const std::wstring& InCommands, uint32 InBaseCommandIndex = 0, bool* OutResultCommand = nullptr );
};

#endif // !BASECOMMANDLET_H
