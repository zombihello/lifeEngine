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
#include "Core.h"

/**
 * @ingroup Core
 * Is char is whitespace
 * 
 * @param[in] InChar Tested char
 * @return Return true if InChar is whitespace, else return false
 */
FORCEINLINE bool appIsWhitespace( tchar InChar )
{
	return InChar == TEXT( ' ' ) || InChar == TEXT( '\t' );
}

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

/**
 * @ingroup Core
 * Parses a string into tokens, separating switches (beginning with - or /) from
 * other parameters
 * 
 * @param[in] InCmdLine The string to parse
 * @param[out] OutTokens Filled with all parameters found in the string
 * @param[out] OutSwitches Filled with all switches found in the string
 */
void appParseCommandLine( const tchar* InCmdLine, std::vector< std::wstring >& OutTokens, std::vector< std::wstring >& OutSwitches );

/**
 * @ingroup Core
 * Return base directory of the game
 * @return Return base directory of the game
 */
FORCEINLINE std::wstring appBaseDir()
{
	return TEXT( "../../" );
}

/**
 * @ingroup Core
 * Return shader directory
 * @return Return shader directory
 */
FORCEINLINE std::wstring appShaderDir()
{
	return TEXT( "../../Engine/Shaders" );
}

#endif // !MISC_H
