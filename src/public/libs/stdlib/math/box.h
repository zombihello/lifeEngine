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

#ifndef BOX_H
#define BOX_H

#include "stdlib/math/math.h"

/**
 * @ingroup stdlib
 * @brief A rectangular minimum bounding volume
 */
class CBox
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CBox()
		: bIsValid( false )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param min	Min position
	 * @param max	Max position
	 */
	FORCEINLINE CBox( const Vector& min, const Vector& max )
		: minLocation( min )
		, maxLocation( max )
		, bIsValid( true )
	{}

	/**
	 * @brief Utility function to build an AABB from InLocation and InSize
	 * 
	 * @param location	Location of AABB
	 * @param size		Size of AABB
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const Vector& location, const Vector& size )
	{
		return CBox( location - size, location + size );
	}

	/**
	 * @brief Utility function to build an AABB from InLocation, InMinXYZ and InMaxXYZ
	 *
	 * @param location	Location of AABB
	 * @param minXYZ	Minimum by XYZ
	 * @param maxXYZ	Maximum by XYZ
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const Vector& location, const Vector& minXYZ, const Vector& maxXYZ )
	{
		return CBox( location + minXYZ, location + maxXYZ );
	}

	/**
	 * @brief Clear bounding box
	 */
	FORCEINLINE void Clear()
	{
		bIsValid = false;
	}

	/**
	 * @brief Get min
	 * @return Return min
	 */
	FORCEINLINE const Vector& GetMin() const
	{
		return minLocation;
	}

	/**
	 * @brief Get max
	 * @return Return max
	 */
	FORCEINLINE const Vector& GetMax() const
	{
		return maxLocation;
	}

	/**
	 * @brief Is valid AABB
	 * @return Return true if AABB is valid, else returning false
	 */
	FORCEINLINE bool IsValid() const
	{
		return bIsValid;
	}

private:
	Vector	minLocation;		/**< Min position */
	Vector	maxLocation;		/**< Max position */
	bool	bIsValid;			/**< Is valid box */
};

#endif // !BOX_H