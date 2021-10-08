/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMPORTTEXTURECOMMANDLET_H
#define IMPORTTEXTURECOMMANDLET_H

#include "Commandlets/BaseCommandlet.h"

/**
 * @ingroup WorldEd
 * Commandlet for import texture to engine
 */
class LImportTextureCommandlet : public LObject
{
	DECLARE_CLASS( LImportTextureCommandlet, LBaseCommandlet )

public:
	/**
	 * Main method of execute commandlet
	 *
	 * @param[in] InCommands Arguments of start commandlet
	 */
	virtual void Main( const std::wstring& InCommand );
};

#endif // !IMPORTTEXTURECOMMANDLET_H