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

 /**
  * @ingroup Core
  * @brief Class for work with string
  */
class ÑString
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
};

#endif // !STRING_H
