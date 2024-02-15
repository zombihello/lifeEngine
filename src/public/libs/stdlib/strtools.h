/**
 * @file
 * @addtogroup stdlib stdlib
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

#ifndef STRTOOLS_H
#define STRTOOLS_H

#include <string>
#include "stdlib/stdlib.h"

// Include platform specific inline functions
#if PLATFORM_WINDOWS
	#include "stdlib/platforms/windows/win_strtools.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup stdlib
 * @brief Get formatted string (for Unicode strings)
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_snwprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vsnwprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup stdlib
 * @brief Get formatted string (for ANSI strings)
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_snprintf( achar* pDest, uint32 maxLen, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup core
 * @brief Printf to string with variable list (for ANSI strings)

 * @param pFormat   String format
 * @param params	Parameters list
 * @return Return formatted string
 */
std::string L_vsprintf( const achar* pFormat, va_list params );

/**
 * @ingroup stdlib
 * @brief Printf to string (for ANSI strings)

 * @param pFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::string L_sprintf( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::string		result = L_vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup core
 * @brief Printf to string with variable list (for Unicode strings)

 * @param pFormat   String format
 * @param params	Parameters list
 * @return Return formatted string
 */
std::wstring L_vswprintf( const wchar* pFormat, va_list params );

/**
 * @ingroup stdlib
 * @brief Printf to string (for Unicode strings)

 * @param pFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::wstring L_swprintf( const wchar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::wstring	result = L_vswprintf( pFormat, params );
	va_end( params );
	return result;
}

#endif // !STRTOOLS_H