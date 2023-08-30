/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSSHAPEGEOMETRY_H
#define PHYSICSSHAPEGEOMETRY_H

#include "System/Archive.h"
#include "System/PhysicsMaterial.h"
#include "Core.h"

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