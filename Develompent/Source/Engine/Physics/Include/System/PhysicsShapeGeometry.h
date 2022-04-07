/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSSHAPEGEOMETRY_H
#define PHYSICSSHAPEGEOMETRY_H

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "System/Archive.h"
#include "System/PhysicsMaterial.h"
#include "Core.h"

/**
 * @ingroup Physics
 * @brief Enumeration of type shape collision
 */
enum ECollisionShape
{
	CS_Box			/**< Collision box */
};

/**
 * @ingroup Physics
 * @brief Base shape used for collision
 */
struct FPhysicsShapeGeometry
{
	/**
	 * @brief Constructor
	 * @param InCollisionShape Collision shape type
	 */
	FPhysicsShapeGeometry( ECollisionShape InCollisionShape );

	/**
	 * @brief Destructor
	 */
	virtual ~FPhysicsShapeGeometry(); 

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive );

	/**
	 * @brief Init PhysX shape
	 */
	virtual void InitPhysXShape() = 0;

	ECollisionShape			collisionShape;		/**< Collision shape */
	physx::PxShape*			pxShape;			/**< PhysX shape. Is is nullptr need before use call InitPhysXShape */
	FPhysicsMaterialRef		material;			/**< Physics material */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, ECollisionShape& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ECollisionShape ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ECollisionShape& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( ECollisionShape ) );
	return InArchive;
}

#endif // !PHYSICSSHAPEGEOMETRY_H