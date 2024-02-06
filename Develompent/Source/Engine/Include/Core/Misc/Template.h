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

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "Core/CoreDefines.h"
#include "Core/Misc/Types.h"

/**
 * @ingroup Core
 * @brief Macro for calculate count elemnts in array
 *
 * @param[in] InArray Array
 */
#define ARRAY_COUNT( InArray )				( sizeof( InArray ) / sizeof( ( InArray )[0] ) )

/**
 * @ingroup Core
 * @brief Macro for calculate offset to member in struct
 *
 * @param InStruc Struct name
 * @param InMember Member name
 */
#define STRUCT_OFFSET( InStruc, InMember )	( ( ptrint ) &( ( InStruc* )0 )->InMember )

/**
 * @ingroup Core
 * @brief TRemoveReference<type> will remove any references from a type.
 */
template <typename T> struct TRemoveReference		{ typedef T Type; };
template <typename T> struct TRemoveReference<T& >	{ typedef T Type; };
template <typename T> struct TRemoveReference<T&&>	{ typedef T Type; };

/**
 * @ingroup Core
 * @brief Find the maximum between two numbers
 * 
 * @param[in] InA First value
 * @param[in] InB Second value
 */
template< typename TType > 
FORCEINLINE TType Max( const TType InA, const TType InB )
{
	return ( InA >= InB ) ? InA : InB;
}

/**
 * @ingroup Core
 * @brief Find the minimum between two numbers
 *
 * @param[in] InA First value
 * @param[in] InB Second value
 */
template< typename TType > 
FORCEINLINE TType Min( const TType InA, const TType InB )
{
	return ( InA <= InB ) ? InA : InB;
}

/**
 * @ingroup Core
 * @brief Aligns a value to the nearest higher multiple of 'Alignment', which must be a power of two
 *
 * @param[in] InPtr	Value to align
 * @param[in] InAlignment Alignment, must be a power of two
 * @return Aligned value
 */
template< typename TType > 
FORCEINLINE TType Align( const TType InPtr, uint64 InAlignment )
{
	return ( TType )( ( ( ptrint )InPtr + InAlignment - 1 ) & ~( InAlignment - 1 ) );
}

/**
 * @ingroup Core
 * Clamp value in range Min and Max
 * 
 * @param[in] InX Value
 * @param[in] InMin Min range
 * @param[in] InMax Max range
 * @return Return clamped value
 */
template< typename TType > 
FORCEINLINE TType Clamp( const TType InX, const TType InMin, const TType InMax )
{
	return InX < InMin ? InMin : InX < InMax ? InX : InMax;
}

/**
 * @ingroup Core
 * Swap InOutA and InOutB
 * 
 * @param InOutA	Value A
 * @param InOutB	Value B
 */
template< typename TType >
FORCEINLINE void Swap( TType& InOutA, TType& InOutB )
{
	const TType		temp = InOutA;
	InOutA = InOutB;
	InOutB = temp;
}

/**
 * @ingroup Core
 * @brief MoveTemp will cast a reference to an rvalue reference.
 * This is lifeEngine equivalent of std::move except that it will not compile when passed an rvalue or
 * const object, because we would prefer to be informed when MoveTemp will have no effect
 * 
 * @param InObj		Object
 */
template <typename T>
FORCEINLINE typename TRemoveReference<T>::Type&& MoveTemp( T&& InObj )
{
	typedef typename TRemoveReference<T>::Type CastType;

	// Validate that we're not being passed an rvalue or a const object - the former is redundant, the latter is almost certainly a mistake
	//static_assert( TIsLValueReferenceType<T>::Value, "MoveTemp called on an rvalue" );
	//static_assert( !TAreTypesEqual<CastType&, const CastType&>::Value, "MoveTemp called on a const object" );

	return ( CastType&& )InObj;
}

#endif // !TEMPLATE_H
