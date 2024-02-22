/**
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

#include "pch_stdlib.h"
#include "stdlib/strtools.h"

/*
==================
L_Vsprintf
==================
*/
std::string L_Vsprintf( const achar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	achar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( achar* )malloc( bufferSize * sizeof( achar ) );

		// Get formated string with args
		result = L_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	std::string		formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
L_Vsprintf
==================
*/
std::wstring L_Vsprintf( const wchar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	wchar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( wchar* )malloc( bufferSize * sizeof( wchar ) );

		// Get formated string with args
		result = L_Vsnprintf( pBuffer, bufferSize, pFormat, params );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	pBuffer[result] = 0;

	std::wstring		formatedString = pBuffer;
	free( pBuffer );
	return formatedString;
}

/*
==================
L_Strncat
==================
*/
achar* L_Strncat( achar* pDest, const achar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32		charsToCopy = 0;
	uint32		destLen = L_Strlen( pDest );
	uint32		srcLen = L_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return pDest;
	}

	// Copy pSrc to pDest
	return strncat( pDest, pSrc, charsToCopy );
}

/*
==================
L_Strncat
==================
*/
wchar* L_Strncat( wchar* pDest, const wchar* pSrc, uint32 destBufferSize, int32 maxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( pDest );
	Assert( pSrc );

	// Get string length of pDest and pSrc
	uint32		charsToCopy = 0;
	uint32		destLen = L_Strlen( pDest );
	uint32		srcLen = L_Strlen( pSrc );

	// Determining whether to copy the entire line or part
	if ( maxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )maxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= destBufferSize )
	{
		charsToCopy = destBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return pDest;
	}

	// Copy pSrc to pDest
	return wcsncat( pDest, pSrc, charsToCopy );
}