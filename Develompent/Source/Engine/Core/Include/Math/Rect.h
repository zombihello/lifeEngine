/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RECT_H
#define RECT_H

#include "Core.h"
#include "Misc/Types.h"
#include "System/Archive.h"

/**
 * @ingroup Core
 * @brief The class for work with rect
 */
template< typename TType >
struct FRect
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE FRect()
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InLeft Coord by X
	 * @param InTop Cooed by Y
	 * @param InWidth Width of rect
	 * @param InHeight Height of rect
	 */
	FORCEINLINE FRect( const TType& InLeft, const TType& InTop, const TType& InWidth, const TType& InHeight )
		: left( InLeft ), top( InTop ), width( InWidth ), height( InHeight )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InCopy Copy of object
	 */
	FORCEINLINE FRect( const FRect< TType >& InCopy )
		: left( InCopy.left ), top( InCopy.top ), width( InCopy.width ), height( InCopy.height )
	{}


	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE FRect< TType >& operator=( const FRect< TType >& InRight )
	{
		left = InRight.left;
		top = InRight.top;
		width = InRight.width;
		height = InRight.height;
		return *this;
	}

	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE const FRect< TType >& operator=( const FRect< TType >& InRight ) const
	{
		left = InRight.left;
		top = InRight.top;
		width = InRight.width;
		height = InRight.height;
		return *this;
	}

	/**
	 * Overload operator for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InArchive, FRect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( FRect<TType> ) );
		return InArchive;
	}

	/**
	 * Overload operator for serialize
	 */
	FORCEINLINE friend FArchive& operator<<( FArchive& InArchive, const FRect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( FRect<TType> ) );
		return InArchive;
	}

	TType		left;		/**< Coord by X */
	TType		top;		/**< Cooed by Y */
	TType		width;		/**< Width of rect */
	TType		height;		/**< Height of rect */
};

/**
 * @ingroup Core
 * @brief Typedef of rect with type int32
 */
typedef FRect< int32 >		FRectInt32;

/**
 * @ingroup Core
 * @brief Typedef of rect with type float
 */
typedef FRect< float >		FRectFloat;

#endif // !COLOR_H
