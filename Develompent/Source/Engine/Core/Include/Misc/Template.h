/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

/**
 * @ingroup Core
 * @brief Macro for calculate count elemnts in array
 *
 * @param[in] InArray Array
 */
#define ARRAY_COUNT( InArray )				( sizeof( InArray ) / sizeof( ( InArray )[0] ) )

/**
 * @ingroup Core
 * @brief Find the maximum between two numbers
 * 
 * @param[in] InA First value
 * @param[in] InB Second value
 */
template< typename TType > 
FORCEINLINE TType Max( const TType& InA, const TType& InB )
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
FORCEINLINE TType Min( const TType& InA, const TType& InB )
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
FORCEINLINE TType Align( const TType InPtr, uint32 InAlignment )
{
	return ( TType )( ( ( int32 )InPtr + InAlignment - 1 ) & ~( InAlignment - 1 ) );
}

#endif // !TEMPLATE_H
