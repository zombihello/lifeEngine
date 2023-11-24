/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "CoreDefines.h"
#include "Misc/Types.h"

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
FORCEINLINE TType Align( const TType InPtr, uint32 InAlignment )
{
	return ( TType )( ( ( int32 )InPtr + InAlignment - 1 ) & ~( InAlignment - 1 ) );
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
 * @brief Inherit from this class to prevent your class from being copied
 */
class CNonCopyable
{
protected:
	/**
	 * @brief Constructor
	 */
	CNonCopyable() {}

	/**
	 * @brief Constructor of move
	 * @param InOther	Other object
	 */
	CNonCopyable( CNonCopyable&& InOther ) {}

	/**
	 * @brief Destructor
	 */
	~CNonCopyable() {}

	/**
	 * @brief Override operator = for move
	 * @param InOther	Other object
	 */
	CNonCopyable& operator=( CNonCopyable&& InOther ) { return *this; }

private:
	/**
	 * @brief Constructor of copy
	 * @param InOther	Other object
	 */
	CNonCopyable( const CNonCopyable& InOther ) {}

	/**
	 * @brief Override operator =
	 * @param InOther	Other object
	 */
	CNonCopyable& operator=( const CNonCopyable& InOther ) { return *this; }
};

/**
 * @ingroup Core
 * @brief Exception-safe guard around saving/restoring a value
 */
template<typename TType>
struct TGuardValue : private CNonCopyable
{
	/**
	 * @brief Constructor
	 * @param InValue		Value to change
	 * @param InNewValue	New value to change
	 */
	TGuardValue( TType& InValue, const TType& InNewValue )
		: value( InValue )
		, oldValue( InValue )
	{
		value = InNewValue;
	}

	/**
	 * @brief Constructor
	 * @param InValue	Value to change
	 */
	TGuardValue( TType& InValue )
		: value( InValue )
		, oldValue( InValue )
	{}

	/**
	 * @brief Destructor
	 */
	~TGuardValue()
	{
		value = oldValue;
	}

	/**
	 * @brief Override dereference operator
	 * @return Return a const reference to the original data value
	 */
	FORCEINLINE const TType& operator*() const
	{
		return oldValue;
	}

private:
	TType&	value;		/**< Value to change */
	TType	oldValue;	/**< Backup of the value */
};

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
