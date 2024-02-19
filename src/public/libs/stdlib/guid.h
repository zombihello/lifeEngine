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

#ifndef GUID_H
#define GUID_H

#include "stdlib/strtools.h"
#include "stdlib/hashing/fasthash.h"

/**
 * @ingroup stdlib
 * @brief Guid
 */
class CGuid
{
public:
	/**
	 * @brief Functions to extract the Guid as a key for std::unordered_map and std::unordered_set
	 */
	struct KeyFunc
	{
		/**
		 * @brief Calculate hash of the Guid
		 *
		 * @param guid	Guid
		 * @return Return hash of this Guid
		 */
		FORCEINLINE std::size_t operator()( const CGuid& guid ) const
		{
			return guid.GetHash();
		}

		/**
		 * @brief Compare CGuid
		 *
		 * @param a		First Guid
		 * @param b		Second Guid
		 * @return Return TRUE if a and b is equal, otherwise will return FALSE
		 */
		FORCEINLINE bool operator()( const CGuid& a, const CGuid& b ) const
		{
			return a < b;
		}
	};

	/**
	 * @brief Constructor
	 */
	CGuid()
		: a( 0 ), b( 0 ), c( 0 ), d( 0 )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param a		A section in Guid
	 * @param b		B section in Guid
	 * @param c		C section in Guid
	 * @param d		D section in Guid
	 */
	CGuid( uint32 a, uint32 b, uint32 c, uint32 d )
		: a( a ), b( b ), c( c ), d( d )
	{}

	/**
	 * @brief Constructor
	 * @param guid	Other Guid
	 */
	CGuid( const CGuid& guid )
		: a( guid.a ), b( guid.b ), c( guid.c ), d( guid.d )
	{}

	/**
	 * @brief Is valid Guid
	 * @return Return TRUE if valid, FALSE otherwise
	 */
	FORCEINLINE bool IsValid() const
	{
		return ( a | b | c | d ) != 0;
	}

	/**
	 * @brief Invalidates the Guid
	 */
	FORCEINLINE void Invalidate()
	{
		a = b = c = d = 0;
	}

	/**
	 * @brief Overrload operator ==
	 */
	FORCEINLINE friend bool operator==( const CGuid& x, const CGuid& y )
	{
		return ( ( x.a ^ y.a ) | ( x.b ^ y.b ) | ( x.c ^ y.c ) | ( x.d ^ y.d ) ) == 0;
	}

	/**
	 * @brief Overrload operator !=
	 */
	FORCEINLINE friend bool operator!=( const CGuid& x, const CGuid& y )
	{
		return ( ( x.a ^ y.a ) | ( x.b ^ y.b ) | ( x.c ^ y.c ) | ( x.d ^ y.d ) ) != 0;
	}

	/**
	 * @brief Operator <
	 */
	FORCEINLINE bool operator<( const CGuid& other ) const
	{
		if ( a < other.a )
		{
			return true;
		}
		if ( b < other.b )
		{
			return true;
		}
		if ( c < other.c )
		{
			return true;
		}
		if ( d < other.d )
		{
			return true;
		}
		return false;
	}

	/**
	 * @brief Set Guid
	 *
	 * @param a		A section in GUID
	 * @param b		B section in GUID
	 * @param c		C section in GUID
	 * @param d		D section in GUID
	 */
	FORCEINLINE void Set( uint32 a, uint32 b, uint32 c, uint32 d )
	{
		a = a;
		b = b;
		c = c;
		d = d;
	}

	/**
	 * @brief Print to string the Guid
	 * @return Return Guid in string format
	 */
	FORCEINLINE std::string String() const
	{
		return L_sprintf( "%08X%08X%08X%08X", a, b, c, d );
	}

	/**
	 * @brief Init Guid from string
	 *
	 * @param stirng	String
	 * @return Return TRUE if Guid is initialized successfully, FALSe otherwise
	 */
	FORCEINLINE bool InitFromString( const std::string& string )
	{
		bool	bSuccessful = false;

		// Size matches, try to parse it
		if ( string.size() == 32 )
		{
			L_sscanf( string.c_str(), "%08X%08X%08X%08X", &a, &b, &c, &d );
			bSuccessful = true;
		}
		// Size mis-match, invalidate the Guid
		else
		{
			Invalidate();
		}
		return bSuccessful;
	}

	/**
	 * @brief Get hash of the Guid
	 * @return Return hash of this Guid
	 */
	FORCEINLINE hash_t GetHash() const
	{
		return FastHash( *this );
	}

private:
	uint32		a;		/**< A section in GUID */
	uint32		b;		/**< B section in GUID */
	uint32		c;		/**< C section in GUID */
	uint32		d;		/**< D section in GUID */
};

#endif // !GUID_H