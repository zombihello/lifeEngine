/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRING_H
#define STRING_H

#include <string>

#include "Misc/Types.h"
#include "CoreDefines.h"

 /**
  * @ingroup Core
  * @brief Class for work with string
  */
class CString
{
public:
	/**
	 * @brief Getting a formatted string
	 * 
	 * @param[in] InFormat Format of string
	 * @param[in] ... Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::wstring			Format( const tchar* InFormat, ... );

	/**
	 * @brief Getting a formatted string
	 *
	 * @param[in] InFormat Format of string
	 * @param[in] InArguments Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::wstring			Format( const tchar* InFormat, va_list InArguments );

	/**
	 * @brief Convert string to upper case
	 * 
	 * @param InString		String
	 * @return Return string in upper case
	 */
	static FORCEINLINE std::wstring ToUpper( const std::wstring& InString )
	{
		std::wstring	result;
		ToUpper( InString, result );
		return result;
	}

	/**
	 * @brief Convert string to upper case
	 * 
	 * @param InString		String
	 * @param OutString		Output string in upper case
	 */
	static FORCEINLINE void ToUpper( const std::wstring& InString, std::wstring& OutString )
	{
		OutString = InString;
		for ( uint32 index = 0, count = OutString.size(); index < count; ++index )
		{
			OutString[index] = std::toupper( OutString[index] );
		}
	}
};

#endif // !STRING_H
