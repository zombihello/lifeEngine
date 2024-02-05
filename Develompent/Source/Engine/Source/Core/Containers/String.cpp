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

#include <memory>

#include "Core/Core.h"
#include "Core/Containers/String.h"

/*
==================
CString::Format
==================
*/
std::wstring CString::Format( const tchar* InFormat, ... )
{
	va_list			arguments;
	va_start( arguments, InFormat );

	std::wstring	formatedString = Format( InFormat, arguments );

	va_end( arguments );
	return formatedString;
}

/*
==================
CString::Format
==================
*/
std::wstring CString::Format( const tchar* InFormat, va_list InArguments )
{
	int32           bufferSize = 1024;
	tchar*			buffer = nullptr;
	int             result = -1;

	while ( result == -1 )
	{
		Memory::Free( buffer );
		buffer = ( tchar* )Memory::Malloc( bufferSize * sizeof( tchar ) );

		// Get formated string with args
		result = Sys_GetVarArgs( buffer, bufferSize, bufferSize - 1, InFormat, InArguments );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	buffer[ result ] = 0;

	std::wstring		formatedString = buffer;
	Memory::Free( buffer );
	return formatedString;
}