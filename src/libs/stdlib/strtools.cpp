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

#include "stdlib/strtools.h"

/*
==================
L_vsprintf
==================
*/
std::string L_vsprintf( const achar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	achar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( achar* )malloc( bufferSize * sizeof( achar ) );

		// Get formated string with args
		result = L_vsnprintf( pBuffer, bufferSize, pFormat, params );
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
L_vswprintf
==================
*/
std::wstring L_vswprintf( const wchar* pFormat, va_list params )
{
	int32	bufferSize = 1024;
	wchar*	pBuffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( pBuffer );
		pBuffer = ( wchar* )malloc( bufferSize * sizeof( wchar ) );

		// Get formated string with args
		result = L_vsnwprintf( pBuffer, bufferSize, pFormat, params );
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