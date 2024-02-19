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

#ifndef RECT_H
#define RECT_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
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
	 * @param left		Coord by X
	 * @param top		Cooed by Y
	 * @param width		Width of rect
	 * @param height	Height of rect
	 */
	FORCEINLINE Rect( const TType& left, const TType& top, const TType& width, const TType& height )
		: left( left )
		, top( top )
		, width( width )
		, height( height )
	{}

	/**
	 * @brief Constructor of copy
	 * @param copy	Copy of object
	 */
	FORCEINLINE Rect( const Rect< TType >& copy )
		: left( copy.left )
		, top( copy.top )
		, width( copy.width )
		, height( copy.height )
	{}


	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE Rect<TType>& operator=( const Rect<TType>& other )
	{
		left = other.left;
		top = other.top;
		width = other.width;
		height = other.height;
		return *this;
	}

	/**
	 * @brief Override of operator copy
	 */
	FORCEINLINE const Rect<TType>& operator=( const Rect<TType>& other ) const
	{
		left = other.left;
		top = other.top;
		width = other.width;
		height = other.height;
		return *this;
	}

	TType	left;		/**< Coord by X */
	TType	top;		/**< Cooed by Y */
	TType	width;		/**< Width of rect */
	TType	height;		/**< Height of rect */
};

/**
 * @ingroup stdlib
 * @brief Typedef of rect with type int32
 */
typedef Rect<int32>		RectInt32_t;

/**
 * @ingroup stdlib
 * @brief Typedef of rect with type float
 */
typedef Rect<float>		RectFloat_t;

#endif // !COLOR_H
