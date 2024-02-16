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

#ifndef WIN_STRTOOLS_INL
#define WIN_STRTOOLS_INL

/**
 * @ingroup stdlib
 * @brief Compare strings without case sensitivity (for ANSI strings)
 * @note Need implement on each platform
 * 
 * @param pString1     String 1 to compare
 * @param pString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_stricmp( const achar* pString1, const achar* pString2 ) 
{ 
	return stricmp( pString1, pString2 ); 
}

/**
 * @ingroup stdlib
 * @brief Compare strings without case sensitivity (for Unicode strings)
 * @note Need implement on each platform
 * 
 * @param pString1     String 1 to compare
 * @param pString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_wcsicmp( const wchar* pString1, const wchar* pString2 )
{ 
	return wcsicmp( pString1, pString2 ); 
}

/**
 * @ingroup stdlib
 * @brief Compares the specified number of characters of two strings without regard to case (for ANSI strings)
 * @note Need implement on each platform
 * 
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @param count       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_strnicmp( const achar* pString1, const achar* pString2, uint32 count ) 
{ 
	return strnicmp( pString1, pString2, count ); 
}

/**
 * @ingroup stdlib
 * @brief Compares the specified number of characters of two strings without regard to case (for Unicode strings)
 *@note Need implement on each platform
 * 
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @param count       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_wcsnicmp( const wchar* pString1, const wchar* pString2, uint32 count )
{
	return wcsnicmp( pString1, pString2, count );
}

#endif // !WIN_STRTOOLS_INL