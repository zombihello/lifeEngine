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

#ifndef PHYSICSSHAPEGEOMETRY_H
#define PHYSICSSHAPEGEOMETRY_H

#include "Core/System/Archive.h"
#include "Core/Core.h"
#include "Physics/System/PhysicsMaterial.h"

/**
 * @ingroup Physics
 * @brief Base shape used for collision
 */
struct PhysicsShapeGeometry
{
	/**
	 * @brief Constructor
	 * @param InCollisionShape Collision shape type
	 */
	PhysicsShapeGeometry( ECollisionShape InCollisionShape );

	/**
	 * @brief Destructor
	 */
	virtual ~PhysicsShapeGeometry(); 

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive );

	/**
	 * @brief Get physics shape handle
	 * @return Return physics shape handle
	 */
	FORCEINLINE const PhysicsShapeHandle_t& GetShapeHandle() const
	{
		if ( !CPhysicsInterface::IsValidShapeGeometry( handle ) )
		{
			InitPhysicsShape();
		}
		return handle;
	}

	ECollisionShape						collisionShape;		/**< Collision shape */
	CollisionProfile*					collisionProfile;	/**< Collision profile */
	TAssetHandle<CPhysicsMaterial>		material;			/**< Physics material */

protected:
	/**
	 * @brief Init physics shape
	 */
	virtual void InitPhysicsShape() const = 0;

	mutable PhysicsShapeHandle_t		handle;				/**< Physics shape */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, ECollisionShape& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ECollisionShape ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ECollisionShape& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( ECollisionShape ) );
	return InArchive;
}

#endif // !PHYSICSSHAPEGEOMETRY_H