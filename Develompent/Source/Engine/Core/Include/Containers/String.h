/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRING_H
#define STRING_H

#include <string>

#include "Misc/Types.h"
#include "CoreDefines.h"

 /**
  * @ingroup Core
  * @brief Class for work with string
  */
class CString
{
public:
	/**
	 * @brief Getting a formatted string (Unicode version)
	 * 
	 * @param InFormat	Format of string
	 * @param ...		Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::wstring Format( const tchar* InFormat, ... );

	/**
	 * @brief Getting a formatted string (Unicode version)
	 *
	 * @param InFormat		Format of string
	 * @param InArguments	Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::wstring Format( const tchar* InFormat, va_list InArguments );

	/**
	 * @brief Getting a formatted string (ANSI version)
	 *
	 * @param InFormat	Format of string
	 * @param ...		Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::string Format( const achar* InFormat, ... );

	/**
	 * @brief Getting a formatted string (ANSI version)
	 *
	 * @param InFormat		Format of string
	 * @param InArguments	Other arguments string
	 * @return Return formated string. This line must be removed manually
	 */
	static std::string Format( const achar* InFormat, va_list InArguments );

	/**
	 * @brief Convert string to upper case
	 * 
	 * @param InString		String
	 * @return Return string in upper case
	 */
	static FORCEINLINE std::wstring ToUpper( const std::wstring& InString )
	{
		std::wstring	result;
		ToUpper( InString, result );
		return result;
	}

	/**
	 * @brief Convert string to upper case
	 * 
	 * @param InString		String
	 * @param OutString		Output string in upper case
	 */
	static FORCEINLINE void ToUpper( const std::wstring& InString, std::wstring& OutString )
	{
		OutString = InString;
		for ( uint32 index = 0, count = OutString.size(); index < count; ++index )
		{
			OutString[index] = std::toupper( OutString[index] );
		}
	}

	/**
	 * @brief Convert string to lower case
	 *
	 * @param InString		String
	 * @return Return string in lower case
	 */
	static FORCEINLINE std::wstring ToLower( const std::wstring& InString )
	{
		std::wstring	result;
		ToLower( InString, result );
		return result;
	}

	/**
	 * @brief Convert string to lower case
	 *
	 * @param InString		String
	 * @param OutString		Output string in lower case
	 */
	static FORCEINLINE void ToLower( const std::wstring& InString, std::wstring& OutString )
	{
		OutString = InString;
		for ( uint32 index = 0, count = OutString.size(); index < count; ++index )
		{
			OutString[index] = std::tolower( OutString[index] );
		}
	}

	/**
	 * @brief Is exist substring in string
	 * 
	 * @param InString			Base string
	 * @param InSubString		Substring in InString
	 * @param InIgnoreRegister	Is need ignore upper/low cases
	 * @return Return TRUE if InSubString is exist, otherwise will return FALSE
	 */
	static FORCEINLINE bool InString( const std::wstring& InString, const std::wstring& InSubString, bool InIgnoreRegister = false )
	{
		if ( !InIgnoreRegister )
		{
			return InString.find( InSubString ) != std::wstring::npos;
		}
		else
		{
			return ToUpper( InString ).find( ToUpper( InSubString ) ) != std::wstring::npos;
		}
	}

	/**
	 * @brief Split string by delimiter on left and right part
	 * 
	 * @param InString			String to split
	 * @param InDelimiter		Delimiter
	 * @param OutLeftPart		Output left part. If the delimiter is not found, return the entire string as the left part
	 * @param OutRightPart		Output right part
	 * @return Return Return TRUE if the delimiter was found, otherwise returns FALSE
	 */
	static FORCEINLINE bool Split( const std::wstring& InString, const std::wstring& InDelimiter, std::wstring& OutLeftPart, std::wstring& OutRightPart )
	{
		std::size_t		idDelimiter = InString.find( InDelimiter );
		if ( idDelimiter != std::wstring::npos )
		{
			OutLeftPart		= InString.substr( 0, idDelimiter );
			OutRightPart	= InString.substr( idDelimiter + 1 );
			return true;
		}

		OutLeftPart = InString;
		return false;
	}

	/**
	 * @brief Split string by delimiter on left and right part
	 *
	 * @param InString			String to split
	 * @param InDelimiter		Delimiter
	 * @param OutLeftPart		Output left part. If the delimiter is not found, return the entire string as the left part
	 * @param OutRightPart		Output right part
	 * @return Return Return TRUE if the delimiter was found, otherwise returns FALSE
	 */
	static FORCEINLINE bool Split( const std::string& InString, const std::string& InDelimiter, std::string& OutLeftPart, std::string& OutRightPart )
	{
		std::size_t		idDelimiter = InString.find( InDelimiter );
		if ( idDelimiter != std::string::npos )
		{
			OutLeftPart		= InString.substr( 0, idDelimiter );
			OutRightPart	= InString.substr( idDelimiter + 1 );
			return true;
		}

		OutLeftPart = InString;
		return false;
	}

	/**
	 * @brief Is string start with
	 * 
	 * @param InString		String to check
	 * @param InPrefix		Prefix
	 * @return Return TRUE if InString start with InPrefix, otherwise returns FALSE
	 */
	static FORCEINLINE bool StartWith( const std::wstring& InString, const std::wstring& InPrefix )
	{
		return InString.compare( 0, InPrefix.size(), InPrefix ) == 0;
	}

	/**
	 * @brief Is string start with
	 *
	 * @param InString		String to check
	 * @param InPrefix		Prefix
	 * @return Return TRUE if InString start with InPrefix, otherwise returns FALSE
	 */
	static FORCEINLINE bool StartWith( const std::string& InString, const std::string& InPrefix )
	{
		return InString.compare( 0, InPrefix.size(), InPrefix ) == 0;
	}
};

#endif // !STRING_H
