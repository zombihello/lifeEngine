/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/13/2021
 */

#ifndef STRING_H
#define STRING_H

#include <string>
#include "Misc/Types.h"

 /**
  * @ingroup Core
  * @brief Class for work with string
  */
class String
{
public:
	/**
	 * @brief Getting a formatted string
	 * 
	 * @param[in] InFormat Format of string
	 * @param[in] ... Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static tchar*			Format( const tchar* InFormat, ... );

	/**
	 * @brief Getting a formatted string
	 *
	 * @param[in] InFormat Format of string
	 * @param[in] InArguments Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static tchar*			Format( const tchar* InFormat, va_list InArguments );
};

#endif // !STRING_H
