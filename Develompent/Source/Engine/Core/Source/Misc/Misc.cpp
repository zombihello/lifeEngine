#include <zlib.h>

#include "Logger/LoggerMacros.h"
#include "Misc/Misc.h"
#include "System/Archive.h"

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

static bool appCompressMemoryZLIB( void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long		zCompressedSize = InOutCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Compress data
	bool		operationSucceeded = compress( ( byte* )InCompressedBuffer, &zCompressedSize, ( const byte* )InUncompressedBuffer, zUncompressedSize ) == Z_OK ? TRUE : FALSE;
	
	// Propagate compressed size from intermediate variable back into out variable.
	InOutCompressedSize = zCompressedSize;
	return operationSucceeded;
}

static bool appUncompressMemoryZLIB( void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Zlib wants to use unsigned long.
	unsigned long		zCompressedSize = InCompressedSize;
	unsigned long		zUncompressedSize = InUncompressedSize;

	// Uncompress data.
	bool		operationSucceeded = uncompress( ( byte* )InUncompressedBuffer, &zUncompressedSize, ( const byte* )InCompressedBuffer, zCompressedSize ) == Z_OK ? TRUE : FALSE;

	// Sanity check to make sure we uncompressed as much data as we expected to.
	check( InUncompressedSize == zUncompressedSize );
	return operationSucceeded;
}

bool appCompressMemory( ECompressionFlags InFlags, void* InCompressedBuffer, uint32& InOutCompressedSize, const void* InUncompressedBuffer, uint32 InUncompressedSize )
{
	// Make sure a valid compression scheme was provided
	check( InFlags | CF_ZLIB );
	bool		compressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		compressSucceeded = appCompressMemoryZLIB( InCompressedBuffer, InOutCompressedSize, InUncompressedBuffer, InUncompressedSize );
		break;

	default:
		LE_LOG( LT_Warning, LC_General, TEXT( "appCompressMemory :: Compression flags 0x%X :: This compression type not supported" ), InFlags );
		compressSucceeded = false;
		break;
	}

	return compressSucceeded;
}

bool appUncompressMemory( ECompressionFlags InFlags, void* InUncompressedBuffer, uint32 InUncompressedSize, const void* InCompressedBuffer, uint32 InCompressedSize )
{
	// Make sure a valid compression scheme was provided
	check( InFlags | CF_ZLIB );
	bool		uncompressSucceeded = false;

	switch ( InFlags )
	{
	case CF_ZLIB:
		uncompressSucceeded = appUncompressMemoryZLIB( InUncompressedBuffer, InUncompressedSize, InCompressedBuffer, InCompressedSize );
		break;

	default:
		LE_LOG( LT_Warning, LC_General, TEXT( "appUncompressMemory :: Compression flags 0x%X :: This compression type not supported" ), InFlags );
		uncompressSucceeded = false;
		break;
	}

	return uncompressSucceeded;
}