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
 * @brief Get formatted string (for Unicode strings)
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param params		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_vsnwprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, va_list params )
{
	return vswprintf( pDest, maxLen, pFormat, params );
}

/**
 * @ingroup stdlib
 * @brief Get formatted string (for ANSI strings)
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param params		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_vsnprintf( achar* pDest, uint32 maxLen, const achar* pFormat, va_list params )
{
	return vsnprintf( pDest, maxLen, pFormat, params );
}

#endif // !WIN_STRTOOLS_INL