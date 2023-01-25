#include "Misc/Misc.h"
#include "Misc/CommandLine.h"

//
// Grab the next space-delimited string from the input stream.
// If quoted, gets entire quoted string.
//
bool ParseToken( const tchar*& InOutStr, std::wstring& OutResult, bool InIsUseEscape )
{
	OutResult.clear();

	// Skip preceeding spaces and tabs.
	while ( appIsWhitespace( *InOutStr ) || *InOutStr == TEXT( '=' ) )
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
			if ( character == 0 || ( ( appIsWhitespace( character ) || character == TEXT( '=' ) ) && !isInQuote ) )
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