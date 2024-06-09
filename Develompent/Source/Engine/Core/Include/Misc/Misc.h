/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MISC_H
#define MISC_H

#include <string>
#include <vector>

#include "Misc/Types.h"
#include "Misc/CoreGlobals.h"
#include "Hashing/FastHash.h"
#include "Core.h"

/**
 * @ingroup Core
 * Return base directory of the game
 * @return Return base directory of the game
 */
FORCEINLINE std::wstring Sys_BaseDir()
{
	return TEXT( ".." ) PATH_SEPARATOR TEXT( ".." ) PATH_SEPARATOR;
}

/**
 * @ingroup Core
 * Return directory of the game
 * @return Return directory of the game
 */
FORCEINLINE std::wstring Sys_GameDir()
{
	return L_Sprintf( TEXT( ".." ) PATH_SEPARATOR TEXT( ".." ) PATH_SEPARATOR TEXT( "%s" ) PATH_SEPARATOR, g_GameName.c_str() );
}

/**
 * @ingroup Core
 * Return shader directory
 * @return Return shader directory
 */
FORCEINLINE std::wstring Sys_ShaderDir()
{
	return TEXT( ".." ) PATH_SEPARATOR TEXT( ".." ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Shaders" ) PATH_SEPARATOR;
}

#endif // !MISC_H
