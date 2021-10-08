#include "Misc/Misc.h"

//
// Grab the next space-delimited string from the input stream.
// If quoted, gets entire quoted string.
//
bool ParseToken( const tchar*& InOutStr, std::wstring& OutResult, bool InIsUseEscape )
{
	OutResult.clear();

	// Skip preceeding spaces and tabs.
	while ( appIsWhitespace( *InOutStr ) )
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
			if ( character == 0 || ( appIsWhitespace( character ) && !isInQuote ) )
			{
				break;
			}
			InOutStr++;

			// Preserve escapes if they're in a quoted string (the check for " is in the else to let \" work as expected)
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

void appParseCommandLine( const tchar* InCmdLine, std::vector< std::wstring >& OutTokens, std::vector< std::wstring >& OutSwitches )
{
	std::wstring		nextToken;
	while ( ParseToken( InCmdLine, nextToken, false ) )
	{
		if ( nextToken[ 0 ] == TEXT( '-' ) || nextToken[ 0 ] == TEXT( '/' ) )
		{
			OutSwitches.push_back( std::wstring( &nextToken[ 1 ], nextToken.size() - 1 ) );
		}
		else
		{
			OutTokens.push_back( nextToken );
		}
	}
}