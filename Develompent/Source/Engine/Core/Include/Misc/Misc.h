/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MISC_H
#define MISC_H

#include <cstring>

#include "Misc/Types.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Checks if a command-line parameter exists in the stream
 * 
 * @param[in] InStream String with command-line parameters
 * @param[in] InParam String a command-line parameter for check
 * @return Return true if InParam exist in InStream, else return false
 */
FORCEINLINE bool				appParseParam( const tchar* InStream, const tchar* InParam )
{
	if ( !InStream || !InParam )
	{
		return false;
	}

	if ( wcsstr( InStream, InParam ) )
	{
		return true;
	}

	return false;
}

#endif // !MISC_H
