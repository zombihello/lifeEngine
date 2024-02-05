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

#include "Core/Misc/Misc.h"
#include "Core/Misc/CommandLine.h"

/*
==================
ParseToken
==================
*/
bool ParseToken( const tchar*& InOutStr, std::wstring& OutResult, bool InIsUseEscape )
{
	//
	// Grab the next space-delimited string from the input stream.
	// If quoted, gets entire quoted string.
	//

	OutResult.clear();

	// Skip preceeding spaces and tabs.
	while ( Sys_IsWhitespace( *InOutStr ) || *InOutStr == TEXT( '=' ) )
	{
		InOutStr++;
	}

	if ( *InOutStr == TEXT( '"' ) )
	{
		// Get quoted string
		InOutStr++;

		while ( *InOutStr && *InOutStr != TEXT( '"' ) )
		{
			tchar		c = *InOutStr++;
			if ( c == '\\' && InIsUseEscape )
			{
				// Get escape
				c = *InOutStr++;
				if ( !c )
				{
					break;
				}
			}

			OutResult += c;
		}

		if ( *InOutStr == TEXT( '"' ) )
		{
			InOutStr++;
		}
	}
	else
	{
		// Get unquoted string (that might contain a quoted part, which will be left intact).
		// For example, -ARG="foo bar baz", will be treated as one token, with quotes intact
		bool			isInQuote = false;

		while ( true )
		{
			tchar		character = *InOutStr;
			if ( character == 0 || ( ( Sys_IsWhitespace( character ) || character == TEXT( '=' ) ) && !isInQuote ) )
			{
				break;
			}
			InOutStr++;

			// Preserve escapes if they're in a quoted string (the Assert for " is in the else to let \" work as expected)
			if ( character == TEXT( '\\' ) && InIsUseEscape && isInQuote )
			{
				OutResult += character;

				character = *InOutStr;
				if ( !character )
				{
					break;
				}
				InOutStr++;
			}
			else if ( character == TEXT( '"' ) )
			{
				isInQuote = !isInQuote;
			}

			OutResult += character;
		}
	}

	return !OutResult.empty();
}

/*
==================
CCommandLine::Parse
==================
*/
void CCommandLine::Parse( const tchar* InCommandLine )
{
	std::wstring		nextToken;
	Params_t::iterator	itCurrentParam = params.end();

	while ( ParseToken( InCommandLine, nextToken, false ) )
	{
		if ( nextToken[0] == TEXT( '-' ) || nextToken[0] == TEXT( '/' ) )
		{
			nextToken = CString::ToUpper( nextToken );
			std::wstring		token( &nextToken[1], nextToken.size()-1 );
			itCurrentParam = params.find( token );
			
			if ( itCurrentParam == params.end() )
			{
				itCurrentParam = params.insert( std::make_pair( token, Values_t() ) ).first;
			}		
		}
		else if ( itCurrentParam != params.end() )
		{
			itCurrentParam->second.push_back( nextToken );
		}
	}
}