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

#ifndef BOX_H
#define BOX_H

#include "Core/Math/Math.h"

/**
 * @ingroup Core
 * @brief A rectangular minimum bounding volume
 */
class CBox
{
public:
	friend CArchive& operator<<( CArchive& InArchive, CBox& InValue );
	friend CArchive& operator<<( CArchive& InArchive, const CBox& InValue );

	/**
	 * Constructor
	 */
	FORCEINLINE CBox()
		: bIsValid( false )
	{}

	/**
	 * Constructor
	 * 
	 * @param InMin Min position
	 * @param InMax Max position
	 */
	FORCEINLINE CBox( const Vector& InMin, const Vector& InMax )
		: minLocation( InMin ), maxLocation( InMax ), bIsValid( true )
	{}

	/**
	 * Utility function to build an AABB from InLocation and InSize
	 * 
	 * @param InLocation Location of AABB
	 * @param InSize Size of AABB
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const Vector& InLocation, const Vector& InSize )
	{
		return CBox( InLocation - InSize, InLocation + InSize );
	}

	/**
	 * Utility function to build an AABB from InLocation, InMinXYZ and InMaxXYZ
	 *
	 * @param InLocation	Location of AABB
	 * @param InMinXYZ		Minimum by XYZ
	 * @param InMaxXYZ		Maximum by XYZ
	 * @return Return created AABB
	 */
	static FORCEINLINE CBox BuildAABB( const Vector& InLocation, const Vector& InMinXYZ, const Vector& InMaxXYZ )
	{
		return CBox( InLocation + InMinXYZ, InLocation + InMaxXYZ );
	}

	/**
	 * Clear bounding box
	 */
	FORCEINLINE void Clear()
	{
		bIsValid = false;
	}

	/**
	 * Get min
	 * @return Return min
	 */
	FORCEINLINE const Vector& GetMin() const
	{
		return minLocation;
	}

	/**
	 * Get max
	 * @return Return max
	 */
	FORCEINLINE const Vector& GetMax() const
	{
		return maxLocation;
	}

	/**
	 * Is valid AABB
	 * @return Return true if AABB is valid, else returning false
	 */
	FORCEINLINE bool IsValid() const
	{
		return bIsValid;
	}

private:
	Vector			minLocation;		/**< Min position */
	Vector			maxLocation;		/**< Max position */
	bool			bIsValid;			/**< Is valid box */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, CBox& InValue )
{
	InArchive << InValue.minLocation;
	InArchive << InValue.maxLocation;
	InArchive << InValue.bIsValid;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CBox& InValue )
{
	InArchive << InValue.minLocation;
	InArchive << InValue.maxLocation;
	InArchive << InValue.bIsValid;
	return InArchive;
}

#endif // !BOX_H