#include <memory>

#include "Core.h"
#include "Containers/String.h"

/**
 * Getting a formatted string
 */
std::wstring ÑString::Format( const tchar* InFormat, ... )
{
	va_list			arguments;
	va_start( arguments, InFormat );

	std::wstring	formatedString = Format( InFormat, arguments );

	va_end( arguments );
	return formatedString;
}

/**
 * Getting a formatted string
 */
std::wstring ÑString::Format( const tchar* InFormat, va_list InArguments )
{
	int32           bufferSize = 1024;
	tchar*			buffer = nullptr;
	int             result = -1;

	while ( result == -1 )
	{
		free( buffer );
		buffer = ( tchar* )malloc( bufferSize * sizeof( tchar ) );

		// Get formated string with args
		result = appGetVarArgs( buffer, bufferSize, bufferSize - 1, InFormat, InArguments );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	buffer[ result ] = 0;

	std::wstring		formatedString = buffer;
	free( buffer );
	return formatedString;
}