/**
 * @file
 * @addtogroup Core Core
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef STRING_H
#define STRING_H

#include <string>

#include "Core/Misc/Types.h"
#include "Core/CoreDefines.h"

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
	static std::wstring Format( const tchar* InFormat, ... );

	/**
	 * @brief Getting a formatted string
	 *
	 * @param[in] InFormat Format of string
	 * @param[in] InArguments Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::wstring Format( const tchar* InFormat, va_list InArguments );

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

	/**
	 * @brief Convert string to lower case
	 *
	 * @param InString		String
	 * @return Return string in lower case
	 */
	static FORCEINLINE std::wstring ToLower( const std::wstring& InString )
	{
		std::wstring	result;
		ToLower( InString, result );
		return result;
	}

	/**
	 * @brief Convert string to lower case
	 *
	 * @param InString		String
	 * @param OutString		Output string in lower case
	 */
	static FORCEINLINE void ToLower( const std::wstring& InString, std::wstring& OutString )
	{
		OutString = InString;
		for ( uint32 index = 0, count = OutString.size(); index < count; ++index )
		{
			OutString[index] = std::tolower( OutString[index] );
		}
	}

	/**
	 * @brief Is exist substring in string
	 * 
	 * @param InString			Base string
	 * @param InSubString		Substring in InString
	 * @param InIgnoreRegister	Is need ignore upper/low cases
	 * @return Return TRUE if InSubString is exist, otherwise will return FALSE
	 */
	static FORCEINLINE bool InString( const std::wstring& InString, const std::wstring& InSubString, bool InIgnoreRegister = false )
	{
		if ( !InIgnoreRegister )
		{
			return InString.find( InSubString ) != std::wstring::npos;
		}
		else
		{
			return ToUpper( InString ).find( ToUpper( InSubString ) ) != std::wstring::npos;
		}
	}
};

#endif // !STRING_H
