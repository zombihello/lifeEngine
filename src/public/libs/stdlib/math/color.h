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

#ifndef COLOR_H
#define COLOR_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
 * @brief The class for work with color
 */
class CColor
{
public:
	/**
	 * @brief Default constructor
	 */
	FORCEINLINE	CColor() :
		r( 0 ),
		g( 0 ),
		b( 0 ),
		a( 255 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param red	Red component of color
	 * @param green	Green component of color
	 * @param blue	Blue component of color
	 * @param alpha	Alpha component of color
	 */
	FORCEINLINE	CColor( uint8 red, uint8 green, uint8 blue, uint8 alpha = 255 )
	{
		Set( red, green, blue, alpha );
	}

	/**
	 * @brief Constructor
	 * @warning Values must be in range [0:1]
	 *
	 * @param linearColor	Linear color
	 */
	FORCEINLINE CColor( const Vector4D& linearColor )
	{
		Set( linearColor );
	}

	/**
	 * @brief Constructor
	 * @param color		Color in uint32 type
	 */
	FORCEINLINE CColor( uint32 color )
	{
		GetUInt32Color() = color;
	}

	/**
	 * @brief Set components of color
	 * 
	 * @param red	Red component of color
	 * @param green	Green component of color
	 * @param blue	Blue component of color
	 * @param alpha	Alpha component of color
	 */
	FORCEINLINE void Set( uint8 red, uint8 green, uint8 blue, uint8 alpha = 255 )
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}

	/**
	 * @brief Set components of color
	 * @warning Values must be in range [0:1]
	 *
	 * @param red	Red component of color
	 * @param green	Green component of color
	 * @param blue	Blue component of color
	 * @param alpha	Alpha component of color
	 */
	FORCEINLINE void Set( float red, float green, float blue, float alpha = 1.f )
	{
		r = ( uint8 )( red * 255.f );
		g = ( uint8 )( green * 255.f );
		b = ( uint8 )( blue * 255.f );
		a = ( uint8 )( alpha * 255.f );
	}

	/**
	 * @brief Set components of color
	 * @warning Values must be in range [0:1]
	 *
	 * @param linearColor	Linear color
	 */
	FORCEINLINE void Set( const Vector4D& linearColor )
	{
		Set( linearColor.x, linearColor.y, linearColor.z, linearColor.a );
	}

	/**
	 * @brief Color in uint32 type
	 * @return Return color in uint32 type
	 */
	FORCEINLINE uint32&	GetUInt32Color() 
	{ 
		return *( uint32* )this;
	}

	/**
	 * @brief Color in uint32 type
	 * @return Return color in uint32 type
	 */
	FORCEINLINE const uint32& GetUInt32Color() const
	{ 
		return *( uint32* )this;
	}

	/**
	 * @brief Convert CColor to Vector4D
	 * @return Return converted CColor to Vector4D
	 */
	FORCEINLINE Vector4D ToVector4D() const
	{
		return Vector4D( r, g, b, a );
	}

	/**
	 * @brief Convert CColor to normalized Vector4D (in range 0 -> 1)
	 * @return Return converted CColor to normalized Vector4D (in range 0 -> 1)
	 */
	FORCEINLINE Vector4D ToNormalizedVector4D() const
	{
		return Vector4D( r / 255.f, g / 255.f, b / 255.f, a / 255.f );
	}

	/**
	 * @brief Override operator ==
	 */
	FORCEINLINE bool operator==( const CColor& other ) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}

	/**
	 * @brief Override operator !=
	 */
	FORCEINLINE bool operator!=( const CColor& other ) const
	{
		return !( *this == other );
	}

	static CColor	black;		/**< Black color */
	static CColor	white;		/**< White color */
	static CColor	red;		/**< Red color */

	uint8			r;			/**< Red component of color */
	uint8			g;			/**< Green component of color */
	uint8			b;			/**< Blue component of color */
	uint8			a;			/**< Alpha component of color */
};

#endif // !COLOR_H
