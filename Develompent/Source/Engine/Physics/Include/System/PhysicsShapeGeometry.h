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
	 * @brief Get physics shape handle
	 * @return Return physics shape handle
	 */
	FORCEINLINE const FPhysicsShapeHandle& GetShapeHandle() const
	{
		if ( !FPhysicsInterface::IsValidShapeGeometry( handle ) )
		{
			InitPhysicsShape();
		}
		return handle;
	}

	ECollisionShape					collisionShape;		/**< Collision shape */	
	FPhysicsMaterialRef				material;			/**< Physics material */

protected:
	/**
	 * @brief Init physics shape
	 */
	virtual void InitPhysicsShape() const = 0;

	mutable FPhysicsShapeHandle		handle;				/**< Physics shape */
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