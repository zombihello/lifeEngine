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
template<typename TType>
struct Rect
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE Rect()
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InLeft Coord by X
	 * @param InTop Cooed by Y
	 * @param InWidth Width of rect
	 * @param InHeight Height of rect
	 */
	FORCEINLINE Rect( const TType& InLeft, const TType& InTop, const TType& InWidth, const TType& InHeight )
		: left( InLeft ), top( InTop ), width( InWidth ), height( InHeight )
	{}

	/**
	 * @brief Constructor of copy
	 * @param InCopy Copy of object
	 */
	FORCEINLINE Rect( const Rect< TType >& InCopy )
		: left( InCopy.left ), top( InCopy.top ), width( InCopy.width ), height( InCopy.height )
	{}


	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE Rect< TType >& operator=( const Rect< TType >& InRight )
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
	FORCEINLINE const Rect< TType >& operator=( const Rect< TType >& InRight ) const
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
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, Rect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( Rect<TType> ) );
		return InArchive;
	}

	/**
	 * Overload operator for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InArchive, const Rect<TType>& InValue )
	{
		InArchive.Serialize( &InValue, sizeof( Rect<TType> ) );
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
typedef Rect<int32>		RectInt32_t;

/**
 * @ingroup Core
 * @brief Typedef of rect with type float
 */
typedef Rect<float>		RectFloat_t;

#endif // !COLOR_H
