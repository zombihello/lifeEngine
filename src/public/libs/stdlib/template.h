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

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "stdlib/stdlib.h"

// Undo any defines
#undef ARRAYSIZE
#undef offsetof

/**
 * @ingroup stdlib
 * @brief Macro for calculate count elements in array
 *
 * @param Array		Array
 */
#define ARRAYSIZE( Array )					( sizeof( Array ) / sizeof( ( Array )[0] ) )

/**
 * @ingroup stdlib
 * @brief Macro for calculate offset to member in struct
 *
 * @param Struc		Struct name
 * @param Member	Member name
 */
#define offsetof( Struc, Member )			( ( ptrint )&( ( Struc* )0 )->Member )

/**
 * @ingroup stdlib
 * @brief Find the maximum between two numbers
 *
 * @param a		First value
 * @param b		Second value
 * @return Return maximum value 'a' or 'b'
 */
template<typename TType>
FORCEINLINE TType Max( const TType a, const TType b )
{
	return ( a >= b ) ? a : b;
}

/**
 * @ingroup stdlib
 * @brief Find the minimum between two numbers
 *
 * @param a		First value
 * @param b		Second value
 * @return Return minimum value 'a' or 'b'
 */
template<typename TType>
FORCEINLINE TType Min( const TType a, const TType b )
{
	return ( a <= b ) ? a : b;
}

/**
 * @ingroup stdlib
 * @brief Aligns a value to the nearest higher multiple of 'alignment', which must be a power of two
 *
 * @param value			Value to align
 * @param alignment		Alignment, must be a power of two
 * @return Aligned value
 */
template<typename TType>
FORCEINLINE TType Align( const TType value, uint32 alignment )
{
	return ( TType )( ( ( ptrint )value + alignment - 1 ) & ~( ( ptrint )alignment - 1 ) );
}

/**
 * @ingroup stdlib
 * @brief Clamp value in range Min and Max
 *
 * @param x		Value
 * @param min	Min range
 * @param max	Max range
 * @return Return clamped value
 */
template<typename TType>
FORCEINLINE TType Clamp( const TType x, const TType min, const TType max )
{
	return x < min ? min : x < max ? x : max;
}

/**
 * @ingroup stdlib
 * @brief Swap A and B
 *
 * @param a		Value A
 * @param b		Value B
 */
template<typename TType>
FORCEINLINE void Swap( TType& a, TType& b )
{
	const TType	temp = a;
	a = b;
	b = temp;
}

#endif // !TEMPLATE_H