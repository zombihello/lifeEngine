/**
 * @file
 * @addtogroup stdlib stdlib
 *
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

#ifndef STRTOOLS_H
#define STRTOOLS_H

#include <string>

#include "core/types.h"
#include "core/platform.h"

// Include platform specific inline functions
#if PLATFORM_WINDOWS
	#include "stdlib/platforms/windows/win_strtools.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup stdlib
 * @brief Get formatted string (for Unicode strings)
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param params		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_vsnwprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, va_list params )
{
	return vswprintf( pDest, maxLen, pFormat, params );
}

/**
 * @ingroup stdlib
 * @brief Get formatted string (for ANSI strings)
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param params		Parameters list
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_vsnprintf( achar* pDest, uint32 maxLen, const achar* pFormat, va_list params )
{
	return vsnprintf( pDest, maxLen, pFormat, params );
}

/**
 * @ingroup stdlib
 * @brief Reads data from string and stores them according to parameters (for Unicode strings)
 * 
 * @param pString	C string that the function processes as its source to retrieve the data
 * @param pFormat	C string that contains a format string
 * @param params	Parameters list
 * @return On success, the function returns the number of items in the argument list successfully filled. This count can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_vswscanf( const wchar* pString, const wchar* pFormat, va_list params )
{
	return vswscanf( pString, pFormat, params );
}

/**
 * @ingroup stdlib
 * @brief Reads data from string and stores them according to parameters (for Unicode strings)
 * 
 * @param pString	C string that the function processes as its source to retrieve the data
 * @param pFormat	C string that contains a format string
 * @param ...		Depending on the pFormat string, the function may expect a sequence of additional arguments, each containing a pointer to allocated storage where the interpretation of the extracted characters is stored with the appropriate type
 * @return On success, the function returns the number of items in the argument list successfully filled. This count can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_swscanf( const wchar* pString, const wchar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vswscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup stdlib
 * @brief Reads data from string and stores them according to parameters (for ANSI strings)
 * 
 * @param pString	C string that the function processes as its source to retrieve the data
 * @param pFormat	C string that contains a format string
 * @param params	Parameters list
 * @return On success, the function returns the number of items in the argument list successfully filled. This count can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_vsscanf( const achar* pString, const achar* pFormat, va_list params )
{
	return vsscanf( pString, pFormat, params );
}

/**
 * @ingroup stdlib
 * @brief Reads data from string and stores them according to parameters (for ANSI strings)
 * 
 * @param pString	C string that the function processes as its source to retrieve the data
 * @param pFormat	C string that contains a format string
 * @param ...		Depending on the pFormat string, the function may expect a sequence of additional arguments, each containing a pointer to allocated storage where the interpretation of the extracted characters is stored with the appropriate type
 * @return On success, the function returns the number of items in the argument list successfully filled. This count can match the expected number of items or be less (even zero) in the case of a matching failure. n the case of an input failure before any data could be successfully interpreted, EOF is returned
 */
FORCEINLINE int32 L_sscanf( const achar* pString, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vsscanf( pString, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup stdlib
 * @brief Get string length (for ANSI strings)
 * 
 * @param pString	String
 * @return Return string length
 */
FORCEINLINE uint32 L_strlen( const achar* pString )
{ 
	return ( uint32 )strlen( pString );
}

/**
 * @ingroup stdlib
 * @brief Get string length (for Unicode strings)
 * 
 * @param pString	String
 * @return Return string length
 */
FORCEINLINE uint32 L_wcslen( const wchar* pString )
{ 
	return ( uint32 )wcslen( pString );
}

/**
 * @ingroup stdlib
 * @brief Copy characters from string (for ANSI strings)
 *
 * @param pDest     Pointer to the destination array where the content is to be copied
 * @param pSrc      C string to be copied
 */
FORCEINLINE void L_strcpy( achar* pDest, const achar* pSrc )
{
	strcpy( pDest, pSrc );
}

/**
 * @ingroup stdlib
 * @brief Copy characters from string (for Unicode strings)
 *
 * @param pDest     Pointer to the destination array where the content is to be copied
 * @param pSrc      C string to be copied
 */
FORCEINLINE void L_wcscpy( wchar* pDest, const wchar* pSrc )
{
	wcscpy( pDest, pSrc );
}

/**
 * @ingroup stdlib
 * @brief Copy characters from string (for ANSI strings)
 *
 * @param pDest     Pointer to the destination array where the content is to be copied
 * @param pSrc      C string to be copied
 * @param maxLen	Maximum number of characters to be copied from pSrc
 */
FORCEINLINE void L_strncpy( achar* pDest, const achar* pSrc, uint32 maxLen )
{
	strncpy( pDest, pSrc, maxLen );
}

/**
 * @ingroup stdlib
 * @brief Copy characters from string (for Unicode strings)
 *
 * @param pDest     Pointer to the destination array where the content is to be copied
 * @param pSrc      C string to be copied
 * @param maxLen	Maximum number of characters to be copied from pSrc
 */
FORCEINLINE void L_wcsncpy( wchar* pDest, const wchar* pSrc, uint32 maxLen )
{
	wcsncpy( pDest, pSrc, maxLen );
}

/**
 * @ingroup stdlib
 * @brief Find substring in a string (for ANSI strings)
 *
 * @param pString     String
 * @param pFind       Find substring in pString
 * @return Return a pointer to the first occurrence of the search string in a string. If not found returns NULL
 */
FORCEINLINE achar* L_strstr( const achar* pString, const achar* pFind ) 
{ 
	return ( achar* )strstr( pString, pFind ); 
}

/**
 * @ingroup stdlib
 * @brief Find substring in a string (for Unicode strings)
 *
 * @param pString     String
 * @param pFind       Find substring in pString
 * @return Return a pointer to the first occurrence of the search string in a string. If not found returns NULL
 */
FORCEINLINE wchar* L_wcsstr( const wchar* pString, const wchar* pFind )
{ 
	return ( wchar* )wcsstr( pString, pFind ); 
}

/**
 * @ingroup stdlib
 * @brief Compares two strings (for ANSI strings)
 *
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_strcmp( const achar* pString1, const achar* pString2 )
{
	return strcmp( pString1, pString2 );
}

/**
 * @ingroup stdlib
 * @brief Compares two strings (for Unicode strings)
 *
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_wcscmp( const wchar* pString1, const wchar* pString2 )
{
	return wcscmp( pString1, pString2 );
}

/**
 * @ingroup stdlib
 * @brief Compares the specified number of characters of two strings (for ANSI strings)
 *
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @param count       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_strncmp( const achar* pString1, const achar* pString2, uint32 count )
{
	return strncmp( pString1, pString2, count );
}

/**
 * @ingroup stdlib
 * @brief Compares the specified number of characters of two strings (for Unicode strings)
 *
 * @param pString1    String 1 to compare
 * @param pString2    String 2 to compare
 * @param count       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - pString1 less than pString2; 0 - pString1 equivalent to pString2; Greater than 0 - pString1 greater than pString2
 */
FORCEINLINE uint32 L_wcsncmp( const wchar* pString1, const wchar* pString2, uint32 count )
{
	return wcsncmp( pString1, pString2, count );
}

/**
 * @ingroup stdlib
 * @brief Convert string to integer (for ANSI strings)
 *
 * @param InString      String to convert
 * @return Return converted string to integer
 */
FORCEINLINE int32 L_atoi( const achar* pString )
{
	return atoi( pString );
}

/**
 * @ingroup stdlib
 * @brief Convert string to integer (for Unicode strings)
 *
 * @param InString      String to convert
 * @return Return converted string to integer
 */
FORCEINLINE int32 L_wtoi( const wchar* pString )
{
	return ( int32 )wcstoul( pString, 0, 10 );
}

/**
 * @ingroup stdlib
 * @brief Get formatted string (for Unicode strings)
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_snwprintf( wchar* pDest, uint32 maxLen, const wchar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vsnwprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup stdlib
 * @brief Get formatted string (for ANSI strings)
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to destination buffer
 * @param maxLen		Maximum string length
 * @param pFormat		String format
 * @param ...			Parameters
 * @return Return number of characters written or -1 if truncated
 */
FORCEINLINE int32 L_snprintf( achar* pDest, uint32 maxLen, const achar* pFormat, ... )
{
	va_list		params;
	va_start( params, pFormat );
	int32		result = L_vsnprintf( pDest, maxLen, pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup core
 * @brief Printf to string with variable list (for ANSI strings)

 * @param pFormat   String format
 * @param params	Parameters list
 * @return Return formatted string
 */
std::string L_vsprintf( const achar* pFormat, va_list params );

/**
 * @ingroup stdlib
 * @brief Printf to string (for ANSI strings)

 * @param pFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::string L_sprintf( const achar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::string		result = L_vsprintf( pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup core
 * @brief Printf to string with variable list (for Unicode strings)

 * @param pFormat   String format
 * @param params	Parameters list
 * @return Return formatted string
 */
std::wstring L_vswprintf( const wchar* pFormat, va_list params );

/**
 * @ingroup stdlib
 * @brief Printf to string (for Unicode strings)

 * @param pFormat	String format
 * @param ...       String parameters
 * @return Return formatted string
 */
FORCEINLINE std::wstring L_swprintf( const wchar* pFormat, ... )
{
	va_list			params;
	va_start( params, pFormat );
	std::wstring	result = L_vswprintf( pFormat, params );
	va_end( params );
	return result;
}

/**
 * @ingroup stdlib
 * @brief Convert string to upper case (for ANSI strings)
 *
 * @param pString	String to convert letters
 * @return Return pointer to pString
 */
FORCEINLINE achar* L_strupper( achar* pString )
{
	achar*	pStr = pString;
	while ( *pStr )
	{
		*pStr = toupper( *pStr );
		++pStr;
	}

	return pString;
}

/**
 * @ingroup stdlib
 * @brief Convert string to lower case (for ANSI strings)
 *
 * @param pString	String to convert letters
 * @return Return pointer to pString
 */
FORCEINLINE achar* L_strlower( achar* pString )
{
	achar*	pStr = pString;
	while ( *pStr )
	{
		*pStr = tolower( *pStr );
		++pStr;
	}

	return pString;
}

/**
 * @ingroup stdlib
 * @brief Convert string to upper case (for Unicode strings)
 *
 * @param pString	String to convert letters
 * @return Return pointer to pString
 */
FORCEINLINE wchar* L_wstrupper( wchar* pString )
{
	wchar*	pStr = pString;
	while ( *pStr )
	{
		*pStr = toupper( *pStr );
		++pStr;
	}

	return pString;
}

/**
 * @ingroup stdlib
 * @brief Convert string to lower case (for Unicode strings)
 *
 * @param pString	String to convert letters
 * @return Return pointer to pString
 */
FORCEINLINE wchar* L_wstrlower( wchar* pString )
{
	wchar*	pStr = pString;
	while ( *pStr )
	{
		*pStr = tolower( *pStr );
		++pStr;
	}

	return pString;
}

/**
 * @ingroup stdlib
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
 * @param c	Character to classify
 * @return Return TRUE if the character is a whitespace character, FALSE otherwise
 */
FORCEINLINE bool L_isspace( achar c )
{
	return isspace( c );
}

/**
 * @ingroup stdlib
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
 * @param c	Character to classify
 * @return Return TRUE if the character is a whitespace character, FALSE otherwise
 */
FORCEINLINE bool L_iswspace( wchar c )
{
	return iswspace( c );
}

/**
 * @ingroup stdlib
 * @brief Class that handles the ANSI to WCHAR conversion
 */
class CANSIToWCHAR_Convert
{
public:
	/**
	 * @brief Converts the string to the desired format
	 *
	 * Converts the string to the desired format. Allocates memory if the
	 * specified destination buffer isn't large enough
	 *
	 * @param pSrcData	The source string to convert
	 * @param pDstData	The destination buffer that holds the converted data
	 * @param size		The size of the dest buffer in bytes
	 * @return Return converted string
	 */
	FORCEINLINE wchar* Convert( const achar* pSrcData, wchar* pDstData, uint32 size ) const
	{
		// Determine whether we need to allocate memory or not
		uint32		length = ( uint32 )L_strlen( pSrcData ) + 1;
		if ( length > size )
		{
			// Need to allocate memory because the string is too big
			pDstData = new wchar[length * sizeof( wchar )];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < length; ++index )
		{
			pDstData[index] = ( byte )pSrcData[index];
		}

		pDstData[length] = '\0';
		return pDstData;
	}

	/**
	 * @brief Get length of string
	 * 
	 * @param pData		Pointer to string for gets it's length
	 * @return Return the string length without the NULL terminator
	 */
	FORCEINLINE uint32 GetLength( wchar* pData ) const
	{
		return ( uint32 )L_wcslen( pData );
	}
};

/**
 * @ingroup stdlib
 * @brief Class that handles the WCHAR to ANSI conversion
 */
class CWCHARToANSI_Convert
{
public:
	/**
	 * @brief Converts the string to the desired format
	 *
	 * Converts the string to the desired format. Allocates memory if the
	 * specified destination buffer isn't large enough
	 *
	 * @param pSrcData	The source string to convert
	 * @param pDstData	The destination buffer that holds the converted data
	 * @param size		The size of the dest buffer in bytes
	 * @return Return converted string
	 */
	FORCEINLINE achar* Convert( const wchar* pSrcData, achar* pDstData, uint32 size )
	{
		// Determine whether we need to allocate memory or not
		uint32	lengthW = ( uint32 )L_wcslen( pSrcData );

		// Needs to be multiply by sizeof( wchar ) the wide in case each converted char is multibyte
		uint32	lengthA = lengthW * sizeof( wchar );
		if ( lengthA > size )
		{
			// Need to allocate memory because the string is too big
			pDstData = new achar[lengthA * sizeof( achar )];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < lengthW; ++index )
		{
			pDstData[index] = pSrcData[index] & 0xFF;
		}

		pDstData[lengthW] = '\0';
		return pDstData;
	}

	/**
	 * @brief Get length of string
	 * 
	 * @param pData		Pointer to string for gets it's length
	 * @return Return the string length without the NULL terminator
	 */
	FORCEINLINE uint32 GetLength( achar* pData )
	{
		return ( uint32 )L_strlen( pData );
	}
};

/**
 * @ingroup stdlib
 * @brief Class takes one type of string and converts it to another
 */
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize = 128>
class TStringConversion : public TBaseConverter
{
public:
	/**
	 * @brief Converts the data by using the Convert() method on the base class
	 * @param pSrcData	Input data
	 */
	explicit FORCEINLINE TStringConversion( const TConvertFrom* pSrcData )
	{
		if ( pSrcData )
		{
			// Use base class convert method
			pConvertedString = TBaseConverter::Convert( pSrcData, buffer, defaultConversionSize );
		}
		else
		{
			pConvertedString = nullptr;
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~TStringConversion()
	{
		if ( !pConvertedString )
		{
			return;
		}

		// Make the string empty so people don't hold onto pointers
		*pConvertedString = 0;

		// Free the memory if it was allocated in order to do the conversion
		if ( pConvertedString != buffer )
		{
			delete[] pConvertedString;
		}
	}

	/**
	 * @brief Operator to get access to the converted string
	 */
	FORCEINLINE operator TConverTo*() const
	{
		return pConvertedString;
	}

	/**
	 * @brief Get length of string
	 * @return Returns the length of the string in number of CONVERT_TO units, excluding the NULL terminator
	 */
	FORCEINLINE uint32 GetLength() const
	{
		return pConvertedString ? TBaseConverter::GetLength( pConvertedString ) : 0;
	}

private:
	/**
	 * @brief Hide the default ctor
	 */
	FORCEINLINE TStringConversion() 
		: pConvertedString( nullptr )
	{};

	TConverTo	buffer[defaultConversionSize];		/**< Holds the converted data if the size is large enough */
	TConverTo*	pConvertedString;					/**< Points to the converted data. If this pointer doesn't match Buffer, then memory was allocated and needs to be freed */
};

/**
 * @ingroup stdlib
 * @brief Type for conversion from ANSI to WCHAR
 */
typedef TStringConversion<wchar, achar, CANSIToWCHAR_Convert>			ansiToWchar_t;

/**
 * @ingroup stdlib
 * @brief Type for conversion from WCHAR to ANSI
 */
typedef TStringConversion<achar, wchar, CWCHARToANSI_Convert>			wcharToAnsi_t;

/**
 * @ingroup stdlib
 * @brief Convert from ANSI to WCHAR
 *
 * @param String	Input string for conversion
 *
 * Example usage: @code ANSI_TO_WCHAR( "Hello" ) @endcode
 */
#define ANSI_TO_WCHAR( String )					( wchar* )ansiToWchar_t( ( const achar* )String )

 /**
  * @ingroup stdlib
  * @brief Convert from WCHAR to ANSI
  *
  * @param String	Input string for conversion
  *
  * Example usage: @code WCHAR_TO_ANSI( L"Hello" ) @endcode
  */
#define WCHAR_TO_ANSI( String )					( achar* )wcharToAnsi_t( ( const wchar* )String )

#endif // !STRTOOLS_H