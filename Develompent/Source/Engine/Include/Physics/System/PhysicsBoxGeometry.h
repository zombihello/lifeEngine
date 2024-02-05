/**
 * @file
 * @addtogroup Physics Physics
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

#ifndef PHYSICSSBOXGEOMETRY_H
#define PHYSICSSBOXGEOMETRY_H

#include "Core/Math/Math.h"
#include "Physics/System/PhysicsShapeGeometry.h"

/**
 * @ingroup Physics
 * @brief Box shape used for collision
 */
struct PhysicsBoxGeometry : public PhysicsShapeGeometry
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE PhysicsBoxGeometry()
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( Math::vectorZero )
	{}

	/**
	 * @brief Constructor
	 * @param InSize Size by X, Y and Z
	 */
	FORCEINLINE PhysicsBoxGeometry( float InSize )
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( InSize, InSize, InSize )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InSizeX Size by X
	 * @param InSizeY Size by Y
	 * @param InSizeZ Size by Z
	 */
	FORCEINLINE PhysicsBoxGeometry( float InSizeX, float InSizeY, float InSizeZ )
		: PhysicsShapeGeometry( CS_Box )
		, location( Math::vectorZero )
		, rotation( Math::quaternionZero )
		, extent( InSizeX, InSizeY, InSizeZ )
	{}

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, PhysicsBoxGeometry& InBoxGometry )
	{
		InBoxGometry.Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator << for serialize
	 */
	FORCEINLINE friend CArchive& operator<<( CArchive& InAr, const PhysicsBoxGeometry& InBoxGometry )
	{
		Assert( InAr.IsSaving() );
		( ( PhysicsBoxGeometry& )InBoxGometry ).Serialize( InAr );
		return InAr;
	}

	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const PhysicsBoxGeometry& InBoxGometry ) const
	{
		return
			collisionShape == InBoxGometry.collisionShape &&
			material == InBoxGometry.material &&
			location == InBoxGometry.location &&
			rotation == InBoxGometry.rotation &&
			extent == InBoxGometry.extent;
	}

	Vector			location;	/**< Position of the box's origin */
	Quaternion		rotation;	/**< Rotation of the box */
	Vector			extent;		/**< Extent of the box */

protected:
	/**
	 * @brief Init physics shape
	 */
	virtual void InitPhysicsShape() const override;
};

#endif // !PHYSICSSBOXGEOMETRY_H