#include "Misc/StringTools.h"
#include "Core.h"

/*
==================
L_Vsprintf
==================
*/
std::string L_Vsprintf( const achar* InFormat, va_list InParams )
{
	int32	bufferSize = 1024;
	achar*	buffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( buffer );
		buffer = ( achar* )malloc( bufferSize * sizeof( achar ) );

		// Get formated string with args
		result = L_Vsnprintf( buffer, bufferSize, InFormat, InParams );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	buffer[result] = 0;

	std::string		formatedString = buffer;
	free( buffer );
	return formatedString;
}

/*
==================
L_Vsprintf
==================
*/
std::wstring L_Vsprintf( const tchar* InFormat, va_list InParams )
{
	int32	bufferSize = 1024;
	tchar*	buffer = nullptr;
	int		result = -1;

	while ( result == -1 )
	{
		free( buffer );
		buffer = ( tchar* )malloc( bufferSize * sizeof( tchar ) );

		// Get formated string with args
		result = L_Vsnprintf( buffer, bufferSize, InFormat, InParams );
		if ( result >= bufferSize )
		{
			result = -1;
		}

		bufferSize *= 2;
	}
	buffer[result] = 0;

	std::wstring		formatedString = buffer;
	free( buffer );
	return formatedString;
}

/*
==================
L_Strncat
==================
*/
achar* L_Strncat( achar* InOutDest, const achar* InSrc, uint32 InDestBufferSize, int32 InMaxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( InOutDest );
	Assert( InSrc );

	// Get string length of InOutDest and InSrc
	uint32		charsToCopy = 0;
	uint32		destLen = L_Strlen( InOutDest );
	uint32		srcLen = L_Strlen( InSrc );

	// Determining whether to copy the entire line or part
	if ( InMaxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )InMaxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= InDestBufferSize )
	{
		charsToCopy = InDestBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return InOutDest;
	}

	// Copy InSrc to InOutDest
	return strncat( InOutDest, InSrc, charsToCopy );
}

/*
==================
L_Strncat
==================
*/
tchar* L_Strncat( tchar* InOutDest, const tchar* InSrc, uint32 InDestBufferSize, int32 InMaxCharsToCopy /*= COPY_ALL_CHARACTERS*/ )
{
	Assert( InOutDest );
	Assert( InSrc );

	// Get string length of InOutDest and InSrc
	uint32		charsToCopy = 0;
	uint32		destLen = L_Strlen( InOutDest );
	uint32		srcLen = L_Strlen( InSrc );

	// Determining whether to copy the entire line or part
	if ( InMaxCharsToCopy <= COPY_ALL_CHARACTERS )
	{
		charsToCopy = srcLen;
	}
	else
	{
		charsToCopy = Min( ( uint32 )InMaxCharsToCopy, srcLen );
	}

	// Make sure what we won't out of bounds
	if ( destLen + charsToCopy >= InDestBufferSize )
	{
		charsToCopy = InDestBufferSize - destLen - 1;
	}

	// If nothing to copy exit
	if ( ( int32 )charsToCopy <= 0 )
	{
		return InOutDest;
	}

	// Copy InSrc to InOutDest
	return wcsncat( InOutDest, InSrc, charsToCopy );
}