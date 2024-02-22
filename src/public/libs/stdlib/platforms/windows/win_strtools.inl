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

#ifndef WIN_STRTOOLS_H
#define WIN_STRTOOLS_H

/*
==================
L_Stricmp
==================
*/
FORCEINLINE uint32 L_Stricmp( const achar* pString1, const achar* pString2 ) 
{ 
	return stricmp( pString1, pString2 ); 
}

/*
==================
L_Stricmp
==================
*/
FORCEINLINE uint32 L_Stricmp( const wchar* pString1, const wchar* pString2 )
{ 
	return wcsicmp( pString1, pString2 ); 
}

/*
==================
L_Strnicmp
==================
*/
FORCEINLINE uint32 L_Strnicmp( const achar* pString1, const achar* pString2, uint32 count ) 
{ 
	return strnicmp( pString1, pString2, count ); 
}

/*
==================
L_Strnicmp
==================
*/
FORCEINLINE uint32 L_Strnicmp( const wchar* pString1, const wchar* pString2, uint32 count )
{
	return wcsnicmp( pString1, pString2, count );
}

#endif // !WIN_STRTOOLS_H