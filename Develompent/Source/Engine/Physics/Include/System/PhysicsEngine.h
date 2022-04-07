/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "Math/Transform.h"
#include "Math/Rotator.h"
#include "System/PhysicsMaterial.h"
#include "Core.h"

/**
 * @ingroup Physics
 * @brief Convert LE FQuaternion to PhysX PxQuat
 * 
 * @param InQuat LE quaternion
 */
FORCEINLINE physx::PxQuat LE2PQuat( const FQuaternion& InQuat )
{
	return physx::PxQuat( InQuat.x, InQuat.y, InQuat.z, InQuat.w );
}

/**
 * @ingroup Physics
 * @brief Convert LE FVector to PhysX PxVec3
 *
 * @param InVec LE vector
 */
FORCEINLINE physx::PxVec3 LE2PVector( const FVector& InVec )
{
	return physx::PxVec3( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert LE FVector4D to PhysX PxVec4
 *
 * @param InVec LE vector 4D
 */
FORCEINLINE physx::PxVec4 LE2PVector( const FVector4D& InVec )
{
	return physx::PxVec4( InVec.x, InVec.y, InVec.z, InVec.w );
}

/**
 * @ingroup Physics
 * @brief Convert LE FTransform to PhysX PxTransform
 *
 * @param InTransform LE transform
 */
FORCEINLINE physx::PxTransform LE2PTransform( const FTransform& InTransform )
{
	physx::PxQuat	pxQuat	= LE2PQuat( InTransform.GetRotation().ToQuaternion() );
	physx::PxVec3	pxPos	= LE2PVector( InTransform.GetLocation() );

	physx::PxTransform		result( pxPos, pxQuat );
	return result;
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxQuat to LE FQuaternion
 *
 * @param InQuat PhysX quaternion
 */
FORCEINLINE FQuaternion P2LEQuat( const physx::PxQuat& InQuat )
{
	return FQuaternion( InQuat.w, InQuat.x, InQuat.y, InQuat.z );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxVec3 to LE FVector
 *
 * @param InVec PhysX vector
 */
FORCEINLINE FVector P2LEVector( const physx::PxVec3& InVec )
{
	return FVector( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxVec4 to LE FVector4D
 *
 * @param InVec PhysX vector 4D
 */
FORCEINLINE FVector4D P2LEVector( const physx::PxVec4& InVec )
{
	return FVector4D( InVec.x, InVec.y, InVec.z, InVec.w );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxTransform to LE FTransform
 *
 * @param InTransform PhysX transform
 */
FORCEINLINE FTransform P2LETransform( const physx::PxTransform& InTransform )
{
	FQuaternion		leQuat	= P2LEQuat( InTransform.q );
	FVector			lePos	= P2LEVector( InTransform.p );

	FTransform		result( FRotator( leQuat ), lePos );
	return result;
}

 /**
  * @ingroup Physics
  * @brief Main class of physics engine
  */
class FPhysicsEngine
{
public:
	/**
	 * @brief Constructor
	 */
	FPhysicsEngine();

	/**
	 * @brief Destructor
	 */
	~FPhysicsEngine();

	/**
	 * @brief Initialize engine
	 */
	void Init();

	/**
	 * @brief Tick engine
	 */
	void Tick();

	/**
	 * @brief Shutdown engine
	 */
	void Shutdown();

	/**
	 * @brief Get PhysX core
	 * @return Return PhysX core
	 */
	FORCEINLINE physx::PxPhysics* GetPxPhysics() const
	{
		return pxPhysics;
	}

	/**
	 * @brief Get default physics material
	 * @return Return default physics material
	 */
	FORCEINLINE FPhysicsMaterialRef GetDefaultPhysMaterial() const
	{
		return defaultPhysMaterial;
	}

private:
	physx::PxFoundation*			pxFoundation;					/**< PhysX foundation */
	physx::PxPhysics*				pxPhysics;						/**< PhysX core */
	physx::PxDefaultAllocator		pxDefaultAllocatorCallback;		/**< Default allocator callback from PhysX */
	physx::PxDefaultErrorCallback	pxDefaultErrorCallback;			/**< Default error callback from PhysX */
	FPhysicsMaterialRef				defaultPhysMaterial;			/**< Default physics material */

#if !SHIPPING_BUILD
	physx::PxPvd*					pxPvd;							/**< Interface for debug PhysX from PVD debugger */
#endif // !SHIPPING_BUILD
};

#endif // !PHYSICSENGINE_H