/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

#include <string>

#include "Misc/Types.h"
#include "Misc/Platform.h"

/**
 * @ingroup Core
 */
enum
{
	COPY_ALL_CHARACTERS = -1		/**< Used in L_Strncat for copy all characters from source string */
};

/**
 * @ingroup Core
 * @brief Get formatted string (for Unicode strings)
 *
 * @param InOutDest		Pointer to destination buffer
 * @param InMaxLen		Maximum string length
 * @param InFormat		String format
 * @param InParams		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_Vsnprintf( tchar* InOutDest, uint32 InMaxLen, const tchar* InFormat, va_list InParams )
{
	return vswprintf( InOutDest, InMaxLen, InFormat, InParams );
}

/**
 * @ingroup Core
 * @brief Get formatted string (for ANSI strings)
 *
 * @param InOutDest		Pointer to destination buffer
 * @param InMaxLen		Maximum string length
 * @param InFormat		String format
 * @param InParams		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_Vsnprintf( achar* InOutDest, uint32 InMaxLen, const achar* InFormat, va_list InParams )
{
	return vsnprintf( InOutDest, InMaxLen, InFormat, InParams );
}

/**
 * @ingroup Core
 * @brief Reads data from string and stores them according to parameters (for Unicode strings)
 * 
 * @param InString		C string that the function processes as its source to retrieve the data
 * @param InFormat		C string that contains a format string
 * @param InOutParams	Parameters list
 * @return On success, the function returns the number of items in the argument list successfully filled. This InCount can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_Vsscanf( const tchar* InString, const tchar* InFormat, va_list InOutParams )
{
	return vswscanf( InString, InFormat, InOutParams );
}

/**
 * @ingroup Core
 * @brief Reads data from string and stores them according to parameters (for Unicode strings)
 * 
 * @param InString	C string that the function processes as its source to retrieve the data
 * @param InFormat	C string that contains a format string
 * @param ...		Depending on the InFormat string, the function may expect a sequence of additional arguments, each containing a pointer to allocated storage where the interpretation of the extracted characters is stored with the appropriate type
 * @return On success, the function returns the number of items in the argument list successfully filled. This InCount can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_Sscanf( const tchar* InString, const tchar* InFormat, ... )
{
	va_list		params;
	va_start( params, InFormat );
	int32		result = L_Vsscanf( InString, InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * @brief Reads data from string and stores them according to parameters (for ANSI strings)
 * 
 * @param InString		C string that the function processes as its source to retrieve the data
 * @param InFormat		C string that contains a format string
 * @param InOutParams	Parameters list
 * @return On success, the function returns the number of items in the argument list successfully filled. This InCount can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_Vsscanf( const achar* InString, const achar* InFormat, va_list InOutParams )
{
	return vsscanf( InString, InFormat, InOutParams );
}

/**
 * @ingroup Core
 * @brief Reads data from string and stores them according to parameters (for ANSI strings)
 * 
 * @param InString	C string that the function processes as its source to retrieve the data
 * @param InFormat	C string that contains a format string
 * @param ...		Depending on the InFormat string, the function may expect a sequence of additional arguments, each containing a pointer to allocated storage where the interpretation of the extracted characters is stored with the appropriate type
 * @return On success, the function returns the number of items in the argument list successfully filled. This InCount can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_Sscanf( const achar* InString, const achar* InFormat, ... )
{
	va_list		params;
	va_start( params, InFormat );
	int32		result = L_Vsscanf( InString, InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * @brief Get string length (for ANSI strings)
 * 
 * @param InString	String
 * @return Return string length
 */
FORCEINLINE uint32 L_Strlen( const achar* InString )
{ 
	return ( uint32 )strlen( InString );
}

/**
 * @ingroup Core
 * @brief Get string length (for Unicode strings)
 * 
 * @param InString	String
 * @return Return string length
 */
FORCEINLINE uint32 L_Strlen( const tchar* InString )
{ 
	return ( uint32 )wcslen( InString );
}

/**
 * @ingroup Core
 * @brief Copy characters from string (for ANSI strings)
 *
 * @param InOutDest  Pointer to the destination array where the content is to be copied
 * @param InSrc      C string to be copied
 */
FORCEINLINE void L_Strcpy( achar* InOutDest, const achar* InSrc )
{
	strcpy( InOutDest, InSrc );
}

/**
 * @ingroup Core
 * @brief Copy characters from string (for Unicode strings)
 *
 * @param InOutDest     Pointer to the destination array where the content is to be copied
 * @param InSrc			C string to be copied
 */
FORCEINLINE void L_Strcpy( tchar* InOutDest, const tchar* InSrc )
{
	wcscpy( InOutDest, InSrc );
}

/**
 * @ingroup Core
 * @brief Copy characters from string (for ANSI strings)
 *
 * @param InOutDest     Pointer to the destination array where the content is to be copied
 * @param InSrc			C string to be copied
 * @param InMaxLen		Maximum number of characters to be copied from InSrc
 */
FORCEINLINE void L_Strncpy( achar* InOutDest, const achar* InSrc, uint32 InMaxLen )
{
	strncpy( InOutDest, InSrc, InMaxLen );
}

/**
 * @ingroup Core
 * @brief Copy characters from string (for Unicode strings)
 *
 * @param InOutDest     Pointer to the destination array where the content is to be copied
 * @param InSrc			C string to be copied
 * @param InMaxLen		Maximum number of characters to be copied from InSrc
 */
FORCEINLINE void L_Strncpy( tchar* InOutDest, const tchar* InSrc, uint32 InMaxLen )
{
	wcsncpy( InOutDest, InSrc, InMaxLen );
}

/**
 * @ingroup Core
 * @brief Find substring in a string (for ANSI strings)
 *
 * @param InString     String
 * @param InFind       Find substring in InString
 * @return Return a pointer to the first occurrence of the search string in a string. If not found returns NULL
 */
FORCEINLINE achar* L_Strstr( const achar* InString, const achar* InFind )
{ 
	return ( achar* )strstr( InString, InFind );
}

/**
 * @ingroup Core
 * @brief Find substring in a string (for Unicode strings)
 *
 * @param InString     String
 * @param InFind       Find substring in InString
 * @return Return a pointer to the first occurrence of the search string in a string. If not found returns NULL
 */
FORCEINLINE tchar* L_Strstr( const tchar* InString, const tchar* InFind )
{ 
	return ( tchar* )wcsstr( InString, InFind );
}

/**
 * @ingroup Core
 * @brief Compares two strings (for ANSI strings)
 *
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strcmp( const achar* InString1, const achar* InString2 )
{
	return strcmp( InString1, InString2 );
}

/**
 * @ingroup Core
 * @brief Compares two strings (for Unicode strings)
 *
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strcmp( const tchar* InString1, const tchar* InString2 )
{
	return wcscmp( InString1, InString2 );
}

/**
 * @ingroup Core
 * @brief Compares the specified number of characters of two strings (for ANSI strings)
 *
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @param InCount      Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strncmp( const achar* InString1, const achar* InString2, uint32 InCount )
{
	return strncmp( InString1, InString2, InCount );
}

/**
 * @ingroup Core
 * @brief Compares the specified number of characters of two strings (for Unicode strings)
 *
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @param InCount      Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strncmp( const tchar* InString1, const tchar* InString2, uint32 InCount )
{
	return wcsncmp( InString1, InString2, InCount );
}

/**
 * @ingroup Core
 * @brief Compare strings without case sensitivity (for ANSI strings)
 * @note Need implement on each platform
 * 
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Stricmp( const achar* InString1, const achar* InString2 );

/**
 * @ingroup Core
 * @brief Compare strings without case sensitivity (for Unicode strings)
 * @note Need implement on each platform
 * 
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Stricmp( const tchar* InString1, const tchar* InString2 );

/**
 * @ingroup Core
 * @brief Compares the specified number of characters of two strings without regard to case (for ANSI strings)
 * @note Need implement on each platform
 * 
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @param InCount      Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strnicmp( const achar* InString1, const achar* InString2, uint32 InCount );

/**
 * @ingroup Core
 * @brief Compares the specified number of characters of two strings without regard to case (for Unicode strings)
 * @note Need implement on each platform
 * 
 * @param InString1    String 1 to compare
 * @param InString2    String 2 to compare
 * @param InCount      Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 L_Strnicmp( const tchar* InString1, const tchar* InString2, uint32 InCount );

/**
 * @ingroup Core
 * @brief Appends the first InMaxCharsToCopy characters of InSrc to InOutDest, plus a terminating null-character (ANSI)
 * 
 * Appends the first InMaxCharsToCopy characters of InSrc to InOutDest, plus a terminating null-character.
 * If InCount of characters to copy more then InDestBufferSize then we copy as we can
 * 
 * @param InOutDest			Destination buffer
 * @param InSrc				String to append
 * @param InDestBufferSize	Size of the buffer pointed to by InOutDest
 * @param InMaxCharsToCopy	Max characters to copy from InSrc. If is COPY_ALL_CHARACTERS will copy whole InSrc
 * @return Return pointer to InOutDest
 */
achar* L_Strncat( achar* InOutDest, const achar* InSrc, uint32 InDestBufferSize, int32 InMaxCharsToCopy = COPY_ALL_CHARACTERS );

/**
 * @ingroup Core
 * @brief Appends the first InMaxCharsToCopy characters of InSrc to InOutDest, plus a terminating null-character (Unicode)
 * 
 * Appends the first InMaxCharsToCopy characters of InSrc to InOutDest, plus a terminating null-character.
 * If InCount of characters to copy more then InDestBufferSize then we copy as we can
 *
 * @param InOutDest			Destination buffer
 * @param InSrc				String to append
 * @param InDestBufferSize	Size of the buffer pointed to by InOutDest
 * @param InMaxCharsToCopy	Max characters to copy from InSrc. If is COPY_ALL_CHARACTERS will copy whole InSrc
 * @return Return pointer to InOutDest
 */
tchar* L_Strncat( tchar* InOutDest, const tchar* InSrc, uint32 InDestBufferSize, int32 InMaxCharsToCopy = COPY_ALL_CHARACTERS );

/**
 * @ingroup Core
 * @brief Convert string to integer (for ANSI strings)
 *
 * @param InString      String to convert
 * @return Return converted string to integer
 */
FORCEINLINE int32 L_Atoi( const achar* InString )
{
	return atoi( InString );
}

/**
 * @ingroup Core
 * @brief Convert string to integer (for Unicode strings)
 *
 * @param InString      String to convert
 * @return Return converted string to integer
 */
FORCEINLINE int32 L_Atoi( const tchar* InString )
{
	return ( int32 )wcstoul( InString, 0, 10 );
}

/**
 * @ingroup Core
 * @brief Convert string to float (for ANSI strings)
 *
 * @param InString      String to convert
 * @return Return converted string to float
 */
FORCEINLINE float L_Atof( const achar* InString )
{
	return ( float )atof( InString );
}

/**
 * @ingroup Core
 * @brief Convert string to float (for Unicode strings)
 *
 * @param InString      String to convert
 * @return Return converted string to float
 */
FORCEINLINE float L_Atof( const tchar* InString )
{
	return wcstof( InString, 0 );
}

/**
 * @ingroup Core
 * @brief Get formatted string (for Unicode strings)
 *
 * @param InOutDest		Pointer to destination buffer
 * @param InMaxLen		Maximum string length
 * @param InFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_Snprintf( tchar* InOutDest, uint32 InMaxLen, const tchar* InFormat, ... )
{
	va_list		params;
	va_start( params, InFormat );
	int32		result = L_Vsnprintf( InOutDest, InMaxLen, InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * @brief Get formatted string (for ANSI strings)
 * @note Need implement on each platform
 *
 * @param InOutDest		Pointer to destination buffer
 * @param InMaxLen		Maximum string length
 * @param InFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_Snprintf( achar* InOutDest, uint32 InMaxLen, const achar* InFormat, ... )
{
	va_list		params;
	va_start( params, InFormat );
	int32		result = L_Vsnprintf( InOutDest, InMaxLen, InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * @brief Printf to string with variable list (for ANSI strings)

 * @param InFormat		String format
 * @param InParams		Parameters list
 * @return Return formatted string
 */
std::string L_Vsprintf( const achar* InFormat, va_list InParams );

/**
 * @ingroup Core
 * @brief Printf to string (for ANSI strings)

 * @param InFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::string L_Sprintf( const achar* InFormat, ... )
{
	va_list			params;
	va_start( params, InFormat );
	std::string		result = L_Vsprintf( InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * @brief Printf to string with variable list (for Unicode strings)

 * @param InFormat		String format
 * @param InParams		Parameters list
 * @return Return formatted string
 */
std::wstring L_Vsprintf( const tchar* InFormat, va_list InParams );

/**
 * @ingroup Core
 * @brief Printf to string (for Unicode strings)

 * @param InFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::wstring L_Sprintf( const tchar* InFormat, ... )
{
	va_list			params;
	va_start( params, InFormat );
	std::wstring	result = L_Vsprintf( InFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup Core
 * brief Convert the character to uppercase
 *
 * @param InCh	Character to be converted
 * @return Return uppercase version of InCh or unmodified InCh if no uppercase version
 */
FORCEINLINE int32 L_ToUpper( int32 InCh )
{
	return toupper( InCh );
}

/**
 * @ingroup Core
 * brief Convert the character to lowercase
 * 
 * @param InCh	Character to be converted
 * @return Return lowercase version of InCh or unmodified InCh if no lowercase version
 */
FORCEINLINE int32 L_ToLower( int32 InCh )
{
	return tolower( InCh );
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for ANSI strings)
 *
 * @param InOutString		String to convert letters
 * @return Return pointer to InOutString
 */
FORCEINLINE achar* L_Strupr( achar* InOutString )
{
	achar*		curChar = InOutString;
	while ( *curChar )
	{
		*curChar = L_ToUpper( *curChar );
		++curChar;
	}

	return InOutString;
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for ANSI strings)
 *
 * @param InOutString		String to convert letters
 * @return Return pointer to InOutString
 */
FORCEINLINE achar* L_Strlwr( achar* InOutString )
{
	achar*		curChar = InOutString;
	while ( *curChar )
	{
		*curChar = L_ToLower( *curChar );
		++curChar;
	}

	return InOutString;
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for ANSI strings)
 *
 * @param InString		String to convert letters
 * @param OutString		Output string with upper case
 */
FORCEINLINE void L_Strupr( const std::string& InString, std::string& OutString )
{
	OutString = InString;
	L_Strupr( OutString.data() );
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for ANSI strings)
 *
 * @param InString		String to convert letters
 * @return Return converted string with upper case
 */
FORCEINLINE std::string L_Strupr( const std::string& InString )
{
	std::string		result;
	L_Strupr( InString, result );
	return result;
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for ANSI strings)
 *
 * @param InString		String to convert letters
 * @param OutString		Output string with lower case
 */
FORCEINLINE void L_Strlwr( const std::string& InString, std::string& OutString )
{
	OutString = InString;
	L_Strlwr( OutString.data() );
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for ANSI strings)
 *
 * @param InOutString		String to convert letters
 * @return Return converted string with lower case
 */
FORCEINLINE std::string L_Strlwr( const std::string& InString )
{
	std::string		result;
	L_Strlwr( InString, result );
	return result;
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for Unicode strings)
 *
 * @param InOutString		String to convert letters
 * @return Return pointer to InString
 */
FORCEINLINE tchar* L_Strupr( tchar* InOutString )
{
	tchar*		curChar = InOutString;
	while ( *curChar )
	{
		*curChar = L_ToUpper( *curChar );
		++curChar;
	}

	return InOutString;
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for Unicode strings)
 *
 * @param InOutString		String to convert letters
 * @return Return pointer to InString
 */
FORCEINLINE tchar* L_Strlwr( tchar* InOutString )
{
	tchar*		curChar = InOutString;
	while ( *curChar )
	{
		*curChar = L_ToLower( *curChar );
		++curChar;
	}

	return InOutString;
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for Unicode strings)
 *
 * @param InString		String to convert letters
 * @param OutString		Output string with upper case
 */
FORCEINLINE void L_Strupr( const std::wstring& InString, std::wstring& OutString )
{
	OutString = InString;
	L_Strupr( OutString.data() );
}

/**
 * @ingroup Core
 * @brief Convert string to upper case (for Unicode strings)
 *
 * @param InString		String to convert letters
 * @return Return converted string with upper case
 */
FORCEINLINE std::wstring L_Strupr( const std::wstring& InString )
{
	std::wstring	result;
	L_Strupr( InString, result );
	return result;
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for Unicode strings)
 *
 * @param InString		String to convert letters
 * @param OutString		Output string with lower case
 */
FORCEINLINE void L_Strlwr( const std::wstring& InString, std::wstring& OutString )
{
	OutString = InString;
	L_Strlwr( OutString.data() );
}

/**
 * @ingroup Core
 * @brief Convert string to lower case (for Unicode strings)
 *
 * @param InOutString		String to convert letters
 * @return Return converted string with lower case
 */
FORCEINLINE std::wstring L_Strlwr( const std::wstring& InString )
{
	std::wstring	result;
	L_Strlwr( InString, result );
	return result;
}

/**
 * @ingroup Core
 * @brief Replace substring in string (for Unicode strings)
 * 
 * @param InSrcString			Source string
 * @param InSubStringToReplace	Substring to replace
 * @param InNewSubString		New substring
 * @return Return new string with replaced substring
 */
FORCEINLINE std::wstring L_ReplaceSubString( const std::wstring& InSrcString, const std::wstring& InSubStringToReplace, const std::wstring& InNewSubString )
{
	std::wstring	newString		= InSrcString;
	std::size_t		idPlaceHolder	= newString.find( InSubStringToReplace );
	while ( idPlaceHolder != std::wstring::npos )
	{
		newString.replace( idPlaceHolder, InSubStringToReplace.size(), InNewSubString );
		idPlaceHolder = newString.find( InSubStringToReplace );
	}

	return newString;
}

/**
 * @ingroup Core
 * @brief Replace substring in string (for ANSI strings)
 *
 * @param InSrcString			Source string
 * @param InSubStringToReplace	Substring to replace
 * @param InNewSubString		New substring
 * @return Return new string with replaced substring
 */
FORCEINLINE std::string L_ReplaceSubString( const std::string& InSrcString, const std::string& InSubStringToReplace, const std::string& InNewSubString )
{
	std::string		newString		= InSrcString;
	std::size_t		idPlaceHolder	= newString.find( InSubStringToReplace );
	while ( idPlaceHolder != std::string::npos )
	{
		newString.replace( idPlaceHolder, InSubStringToReplace.size(), InNewSubString );
		idPlaceHolder = newString.find( InSubStringToReplace );
	}

	return newString;
}

/**
 * @ingroup Core
 * @brief Is char is a space (for ANSI chars)
 * 
 * Checks if the given character is whitespace character as classified by the currently installed C locale. 
 * In the default locale, the whitespace characters are the following:
 * * Space (0x20, ' ')
 * * Form feed (0x0C, '\f')
 * * Line feed (0x0A, '\n')
 * * Carriage return (0x0D, '\r')
 * * Horizontal tab (0x09, '\t')
 * * Vertical tab (0x0b, '\v')
 *
 * @param InC	Character to classify
 * @return Return TRUE if the character is a whitespace character, FALSE otherwise
 */
FORCEINLINE bool L_IsSpace( achar InC )
{
	return isspace( InC );
}

/**
 * @ingroup Core
 * @brief Is char is a digit (for ANSI chars)
 * Decimal digits are any of: 0 1 2 3 4 5 6 7 8 9
 * 
 * @param InC   Char to check
 * @return Return TRUE if InC is a digit, otherwise returns FALSE
 */
FORCEINLINE bool L_IsDigit( achar InC )
{
	return isdigit( InC );
}

/**
 * @ingroup Core
 * @brief Is char is a space (for Unicode chars)
 * 
 * Checks if the given character is whitespace character as classified by the currently installed C locale. 
 * In the default locale, the whitespace characters are the following:
 * * Space (0x20, ' ')
 * * Form feed (0x0C, '\f')
 * * Line feed (0x0A, '\n')
 * * Carriage return (0x0D, '\r')
 * * Horizontal tab (0x09, '\t')
 * * Vertical tab (0x0b, '\v')
 *
 * @param InC	Character to classify
 * @return Return TRUE if the character is a whitespace character, FALSE otherwise
 */
FORCEINLINE bool L_IsSpace( tchar InC )
{
	return iswspace( InC );
}

/**
 * @ingroup Core
 * @brief Is char is a digit (for Unicode chars)
 * Decimal digits are any of: 0 1 2 3 4 5 6 7 8 9
 *
 * @param InC   Char to check
 * @return Return TRUE if InC is a digit, otherwise returns FALSE
 */
FORCEINLINE bool L_IsDigit( tchar InC )
{
	return isdigit( InC );
}

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "WindowsStringTools.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !STRINGTOOLS_H