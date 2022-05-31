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
struct SRect
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE SRect()
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InLeft Coord by X
	 * @param InTop Cooed by Y
	 * @param InWidth Width of rect
	 * @param InHeight Height of rect
	 */
	FORCEINLINE SRect( const TType& InLeft, const TType& InTop, const TType& InWidth, const TType& InHeight )
		: left( InLeft ), top( InTop ), width( InWidth ), height( InHeight )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InCopy Copy of object
	 */
	FORCEINLINE SRect( const SRect< TType >& InCopy )
		: left( InCopy.left ), top( InCopy.top ), width( InCopy.width ), height( InCopy.height )
	{}


	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE SRect< TType >& operator=( const SRect< TType >& InRight )
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
	FORCEINLINE const SRect< TType >& operator=( const SRect< TType >& InRight ) const
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
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, SRect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( SRect<TType> ) );
		return InArchive;
	}

	/**
	 * Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, const SRect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( SRect<TType> ) );
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
typedef SRect< int32 >		RectInt32_t;

/**
 * @ingroup Core
 * @brief Typedef of rect with type float
 */
typedef SRect< float >		RectFloat_t;

#endif // !COLOR_H
