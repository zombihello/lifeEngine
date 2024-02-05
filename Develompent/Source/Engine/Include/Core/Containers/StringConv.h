/**
 * @file
 * @addtogroup Core Core
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

#ifndef STRINGCONV_H
#define STRINGCONV_H

#include <string.h>

#include "Core/Core.h"
#include "Core/Misc/Types.h"

/**
 * @ingroup Core
 * @brief Class that handles the ANSI to TCHAR conversion
 */
class CANSIToTCHAR_Convert
{
public:
	/**
	 * @brief Converts the string to the desired format
	 * 
	 * Converts the string to the desired format. Allocates memory if the
	 * specified destination buffer isn't large enough
	 * 
	 * @param[in] InSource The source string to convert
	 * @param[in] InDest The destination buffer that holds the converted data
	 * @param[in] InSize The size of the dest buffer in bytes
	 * @return Return converted string
	 */
	FORCEINLINE tchar* Convert( const achar* InSource, tchar* InDest, uint32 InSize )
	{
		// Determine whether we need to allocate memory or not
		uint32		length = ( uint32 )strlen( InSource ) + 1;

		if ( length > InSize )
		{
			// Need to allocate memory because the string is too big
			InDest = new tchar[ length * sizeof( tchar ) ];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < length; ++index )
		{
			InDest[ index ] = ( byte )InSource[ index ];
		}

		InDest[ length ] = '\0';
		return InDest;
	}

	/**
	 * @brief Get length of string
	 * @return Return the string length without the null terminator
	 */
	FORCEINLINE uint32 GetLength( tchar* InDest )
	{
		return ( uint32 )wcslen( InDest );
	}
};

/**
 * @ingroup Core
 * @brief Class that handles the TCHAR to ANSI conversion
 */
class CTCHARToANSI_Convert
{
public:
	 /**
	  * @brief Converts the string to the desired format
	  *
	  * Converts the string to the desired format. Allocates memory if the
	  * specified destination buffer isn't large enough
	  *
	  * @param[in] InSource The source string to convert
	  * @param[in] InDest The destination buffer that holds the converted data
	  * @param[in] InSize The size of the dest buffer in bytes
	  * @return Return converted string
	  */
	FORCEINLINE achar* Convert( const tchar* InSource, achar* InDest, uint32 InSize )
	{
		// Determine whether we need to allocate memory or not
		uint32		lengthW = ( uint32 )wcslen( InSource );

		// Needs to be 2x the wide in case each converted char is multibyte
		uint32		lengthA = lengthW * 2;
		if ( lengthA > InSize )
		{
			// Need to allocate memory because the string is too big
			InDest = new char[ lengthA * sizeof( achar ) ];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < lengthW; ++index )
		{
			InDest[ index ] = InSource[ index ] & 0xFF;
		}

		InDest[ lengthW ] = '\0';
		return InDest;
	}

	/**
	 * @brief Get length of string
	 * @return Return the string length without the null terminator
	 */
	FORCEINLINE uint32 GetLength( achar* InDest )
	{
		return ( uint32 )strlen( InDest );
	}
};

/**
 * @ingroup Core
 * @brief Class takes one type of string and converts it to another
 * 
 * Class takes one type of string and converts it to another. The class includes a
 * chunk of presized memory of the destination type. If the presized array is
 * too small, it mallocs the memory needed and frees on the class going out of
 * scope. Uses the overloaded cast operator to return the converted data.
 */
template< typename ConverTo, typename ConvertFrom, typename BaseConverter, uint32 defaultConversionSize = 128 >
class TStringConversion : public BaseConverter
{
public:
	/**
	 * @brief Converts the data by using the Convert() method on the base class
	 * 
	 * @param[in] InSource Input data
	 */
	explicit FORCEINLINE TStringConversion( const ConvertFrom* InSource )
	{
		if ( InSource )
		{
			// Use base class convert method
			convertedString = BaseConverter::Convert( InSource, buffer, defaultConversionSize );
		}
		else
		{
			convertedString = nullptr;
		}
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~TStringConversion()
	{
		if ( !convertedString )		return;

		// Make the string empty so people don't hold onto pointers
		*convertedString = 0;

		// Free the memory if it was allocated in order to do the conversion
		if ( convertedString != buffer )
		{
			delete[] convertedString;
		}
	}

	/**
	 * @brief Operator to get access to the converted string
	 */
	FORCEINLINE operator ConverTo* ( void ) const
	{
		return convertedString;
	}

	/**
	 * @brief Get length of string
	 * 
	 * @return returns the length of the string in number of CONVERT_TO units, excluding the NULL terminator
	 */
	FORCEINLINE uint32 GetLength() const
	{
		return convertedString ? BaseConverter::GetLength( convertedString ) : nullptr;
	}

private:
	/**
	 * @brief Hide the default ctor
	 */
	FORCEINLINE TStringConversion() 
		: convertedString( nullptr ) 
	{};

	ConverTo			buffer[ defaultConversionSize ];			/**< Holds the converted data if the size is large enough */
	ConverTo*			convertedString;							/**< Points to the converted data. If this pointer doesn't match Buffer, then memory was allocated and needs to be freed */

};

/**
 * @ingroup Core
 * @brief Typedef for conversion from ANSI to TCHAR
 */
typedef TStringConversion< tchar, achar, CANSIToTCHAR_Convert >			ANSIToTCHAR;

/**
 * @ingroup Core
 * @brief Typedef for conversion from TCHAR to ANSI
 */
typedef TStringConversion< achar, tchar, CTCHARToANSI_Convert >			TCHARToANSI;

/**
 * @ingroup Core
 * @brief Convert from ANSI to TCHAR
 *
 * @param[in] InString Input string for conversion
 *
 * Example usage: @code ANSI_TO_TCHAR( "Hello" ) @endcode
 */
#define ANSI_TO_TCHAR( InString )				( tchar* )ANSIToTCHAR( ( const achar* )InString )

 /**
  * @ingroup Core
  * @brief Convert from TCHAR to ANSI
  *
  * @param[in] InString Input string for conversion
  *
  * Example usage: @code TCHAR_TO_ANSI( TEXT( "Hello" ) ) @endcode
  */
#define TCHAR_TO_ANSI( InString )				( achar* )TCHARToANSI( ( const tchar* )InString )

#endif // !STRINGCONV_H