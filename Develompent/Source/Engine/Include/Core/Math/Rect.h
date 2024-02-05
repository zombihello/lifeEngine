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

#ifndef RECT_H
#define RECT_H

#include "Core/Core.h"
#include "Core/Misc/Types.h"
#include "Core/System/Archive.h"

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
