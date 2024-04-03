/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRINGCONV_H
#define STRINGCONV_H

#include <string.h>

#include "Core.h"
#include "Misc/Types.h"
#include "Misc/StringTools.h"

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
	 * @param InSource	The source string to convert
	 * @param InOutDest The destination buffer that holds the converted data
	 * @param InSize	The size of the dest buffer in bytes
	 * @return Return converted string
	 */
	FORCEINLINE tchar* Convert( const achar* InSource, tchar* InOutDest, uint32 InSize )
	{
		// Determine whether we need to allocate memory or not
		uint32		length = ( uint32 )L_Strlen( InSource ) + 1;
		if ( length > InSize )
		{
			// Need to allocate memory because the string is too big
			InOutDest = new tchar[length * sizeof( tchar )];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < length; ++index )
		{
			InOutDest[index] = ( byte )InSource[index];
		}

		InOutDest[length] = '\0';
		return InOutDest;
	}

	/**
	 * @brief Get length of string
	 * 
	 * @param InString	String
	 * @return Return the string length without the null terminator
	 */
	FORCEINLINE uint32 GetLength( const tchar* InString )
	{
		return ( uint32 )L_Strlen( InString );
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
	  * @param InSource		The source string to convert
	  * @param InOutDest	The destination buffer that holds the converted data
	  * @param InSize		The size of the dest buffer in bytes
	  * @return Return converted string
	  */
	FORCEINLINE achar* Convert( const tchar* InSource, achar* InOutDest, uint32 InSize )
	{
		// Determine whether we need to allocate memory or not
		uint32		lengthW = ( uint32 )L_Strlen( InSource );

		// Needs to be multiply by sizeof( tchar ) the wide in case each converted char is multibyte
		uint32		lengthA = lengthW * sizeof( tchar );
		if ( lengthA > InSize )
		{
			// Need to allocate memory because the string is too big
			InOutDest = new achar[lengthA * sizeof( achar )];
		}

		// Now do the conversion
		for ( uint32 index = 0; index < lengthW; ++index )
		{
			InOutDest[index] = InSource[index] & 0xFF;
		}

		InOutDest[lengthW] = '\0';
		return InOutDest;
	}

	/**
	 * @brief Get length of string
	 * 
	 * @param InString	String
	 * @return Return the string length without the null terminator
	 */
	FORCEINLINE uint32 GetLength( const achar* InString )
	{
		return ( uint32 )L_Strlen( InString );
	}
};

/**
 * @ingroup Core
 * @brief Class takes one type of string and converts it to another
 */
template<typename TConverTo, typename TConvertFrom, typename TBaseConverter, uint32 defaultConversionSize = 128>
class TStringConversion : public TBaseConverter
{
public:
	/**
	 * @brief Converts the data by using the Convert() method on the base class
	 * @param InSource		Input data
	 */
	explicit FORCEINLINE TStringConversion( const TConvertFrom* InSource )
	{
		if ( InSource )
		{
			// Use base class convert method
			convertedString = TBaseConverter::Convert( InSource, buffer, defaultConversionSize );
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
		if ( !convertedString )
		{
			return;
		}

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
	FORCEINLINE operator TConverTo*() const
	{
		return convertedString;
	}

	/**
	 * @brief Get length of string
	 * @return Returns the length of the string in number of CONVERT_TO units, excluding the NULL terminator
	 */
	FORCEINLINE uint32 GetLength() const
	{
		return convertedString ? TBaseConverter::GetLength( convertedString ) : 0;
	}

private:
	/**
	 * @brief Hide the default ctor
	 */
	FORCEINLINE TStringConversion() 
		: convertedString( nullptr ) 
	{};

	TConverTo	buffer[defaultConversionSize];			/**< Holds the converted data if the size is large enough */
	TConverTo*	convertedString;						/**< Points to the converted data. If this pointer doesn't match Buffer, then memory was allocated and needs to be freed */

};

/**
 * @ingroup Core
 * @brief Typedef for conversion from ANSI to TCHAR
 */
typedef TStringConversion<tchar, achar, CANSIToTCHAR_Convert>			ANSIToTCHAR;

/**
 * @ingroup Core
 * @brief Typedef for conversion from TCHAR to ANSI
 */
typedef TStringConversion<achar, tchar, CTCHARToANSI_Convert>			TCHARToANSI;

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