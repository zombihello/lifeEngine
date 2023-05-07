/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSINTERFACEPHYSX_H
#define PHYSICSINTERFACEPHYSX_H

#if WITH_PHYSX
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "Math/Math.h"
#include "Math/Transform.h"
#include "Math/Rotator.h"
#include "Misc/PhysXGlobals.h"
#include "CoreDefines.h"

/**
 * @ingroup Physics
 * @brief Convert LE Quaternion to PhysX PxQuat
 *
 * @param InQuat LE quaternion
 */
FORCEINLINE physx::PxQuat LE2PQuat( const Quaternion& InQuat )
{
	return physx::PxQuat( InQuat.x, InQuat.y, InQuat.z, InQuat.w );
}

/**
 * @ingroup Physics
 * @brief Convert LE Vector to PhysX PxVec3
 *
 * @param InVec LE vector
 */
FORCEINLINE physx::PxVec3 LE2PVector( const Vector& InVec )
{
	return physx::PxVec3( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert LE Vector4D to PhysX PxVec4
 *
 * @param InVec LE vector 4D
 */
FORCEINLINE physx::PxVec4 LE2PVector( const Vector4D& InVec )
{
	return physx::PxVec4( InVec.x, InVec.y, InVec.z, InVec.w );
}

/**
 * @ingroup Physics
 * @brief Convert LE CTransform to PhysX PxTransform
 *
 * @param InTransform LE transform
 */
FORCEINLINE physx::PxTransform LE2PTransform( const CTransform& InTransform )
{
	physx::PxQuat	pxQuat = LE2PQuat( InTransform.GetRotation().ToQuaternion() );
	physx::PxVec3	pxPos = LE2PVector( InTransform.GetLocation() );

	physx::PxTransform		result( pxPos, pxQuat );
	return result;
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxQuat to LE Quaternion
 *
 * @param InQuat PhysX quaternion
 */
FORCEINLINE Quaternion P2LEQuat( const physx::PxQuat& InQuat )
{
	return Quaternion( InQuat.w, InQuat.x, InQuat.y, InQuat.z );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxVec3 to LE Vector
 *
 * @param InVec PhysX vector
 */
FORCEINLINE Vector P2LEVector( const physx::PxVec3& InVec )
{
	return Vector( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxVec4 to LE Vector4D
 *
 * @param InVec PhysX vector 4D
 */
FORCEINLINE Vector4D P2LEVector( const physx::PxVec4& InVec )
{
	return Vector4D( InVec.x, InVec.y, InVec.z, InVec.w );
}

/**
 * @ingroup Physics
 * @brief Convert PhysX PxTransform to LE CTransform
 *
 * @param InTransform PhysX transform
 */
FORCEINLINE CTransform P2LETransform( const physx::PxTransform& InTransform )
{
	Quaternion		leQuat = P2LEQuat( InTransform.q );
	Vector			lePos = P2LEVector( InTransform.p );

	CTransform		result( CRotator( leQuat ), lePos );
	return result;
}

/**
 * @ingroup Physics
 * @brief Convert lock flags from LE to PhysX
 */
FORCEINLINE physx::PxRigidDynamicLockFlags LE2PLockFlags( uint32 InLockFlags )
{
	physx::PxRigidDynamicLockFlags		pxFlags;

	// Lock moving by X
	if ( InLockFlags & BLF_LockMoveX )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X;
	}

	// Lock moving by Y
	if ( InLockFlags & BLF_LockMoveY )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y;
	}

	// Lock moving by Z
	if ( InLockFlags & BLF_LockMoveZ )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z;
	}

	// Lock rotating by X
	if ( InLockFlags & BLF_LockRotateX )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X;
	}

	// Lock rotating by Y
	if ( InLockFlags & BLF_LockRotateY )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y;
	}

	// Lock rotating by Z
	if ( InLockFlags & BLF_LockRotateZ )
	{
		pxFlags |= physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z;
	}

	return pxFlags;
}

/**
 * @ingroup Physics
 * @brief Handle of material for PhysX
 */
struct SPhysicsMaterialHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	SPhysicsMaterialHandlePhysX()
		: pxMaterial( nullptr )
	{}

	physx::PxMaterial*		pxMaterial;		/**< PhysX material */
};

/**
 * @ingroup Physics
 * @brief Handle of shape for PhysX
 */
struct SPhysicsShapeHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	SPhysicsShapeHandlePhysX()
		: pxShape( nullptr )
	{}

	physx::PxShape*		pxShape;		/**< PhysX shape */
};

/**
 * @ingroup Physics
 * @brief Handle of actor for PhysX
 */
struct SPhysicsActorHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	SPhysicsActorHandlePhysX()
		: bStatic( false )
		, pxRigidActor( nullptr )
	{}

	bool						bStatic;			/**< Is static actor */
	physx::PxRigidActor*		pxRigidActor;		/**< PhysX rigid actor */
};

/**
 * @ingroup Physics
 * @brief PhysX interface
 */
struct SPhysicsInterfacePhysX
{
	/**
	 * @brief Init physics
	 */
	static void Init();

	/**
	 * @brief Shutdown physics
	 */
	static void Shutdown();

	/**
	 * @brief Create box shape
	 * 
	 * @param InSize Size box
	 * @param InPhysMaterial Physics Material
	 * @return Return shape handle
	 */
	static FORCEINLINE SPhysicsShapeHandlePhysX CreateBoxGeometry( const Vector& InSize, const SPhysicsMaterialHandlePhysX& InPhysMaterial )
	{
		SPhysicsShapeHandlePhysX		shapeHandle;
		shapeHandle.pxShape = g_PhysXSDK->createShape( physx::PxBoxGeometry( InSize.x, InSize.y, InSize.z ), *InPhysMaterial.pxMaterial );
		return shapeHandle;
	}

	/**
	 * @brief Release shape
	 * @param InShapeHandle Shape handle
	 */
	static FORCEINLINE void ReleaseShapeGeometry( SPhysicsShapeHandlePhysX& InShapeHandle )
	{
		if ( InShapeHandle.pxShape )
		{
			InShapeHandle.pxShape->release();
			InShapeHandle.pxShape = nullptr;
		}
	}

	/**
	 * @brief Is valid shape
	 *
	 * @param InShapeHandle Shape handle
	 * @return Return true if shape handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidShapeGeometry( const SPhysicsShapeHandlePhysX& InShapeHandle )
	{
		return InShapeHandle.pxShape;
	}

	/**
	 * @brief Create material
	 * 
	 * @param InPhysMaterial Physics material
	 * @return Return material handle
	 */
	static SPhysicsMaterialHandlePhysX CreateMaterial( class CPhysicsMaterial* InPhysMaterial );

	/**
	 * @brief Update material
	 *
	 * @param InMaterialHandle Handle of phys material
	 * @param InPhysMaterial Physics material
	 */
	static void UpdateMaterial( const SPhysicsMaterialHandlePhysX& InMaterialHandle, class CPhysicsMaterial* InPhysMaterial );

	/**
	 * @brief Release material
	 * @param InMaterialHandle Handle of phys material
	 */
	static FORCEINLINE void ReleaseMaterial( SPhysicsMaterialHandlePhysX& InMaterialHandle )
	{
		if ( InMaterialHandle.pxMaterial )
		{
			InMaterialHandle.pxMaterial->release();
			InMaterialHandle.pxMaterial = nullptr;
		}
	}

	/**
	 * @brief Is valid material
	 *
	 * @param InMaterialHandle Material handle
	 * @return Return true if material handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidMaterial( const SPhysicsMaterialHandlePhysX& InMaterialHandle )
	{
		return InMaterialHandle.pxMaterial;
	}

	/**
	 * @brief Create actor
	 * 
	 * @param InParams Actor params for create
	 * @return Return actor handle
	 */
	static SPhysicsActorHandlePhysX CreateActor( const SActorCreationParams& InParams );

	/**
	 * @brief Computation of mass properties for a physics actor
	 * 
	 * @param InActorHandle Handle of physics actor
	 * @param InDensity The density of the body. Used to compute the mass of the body. The density must be greater than 0.
	 * @param InMassLocalPose The center of mass relative to the actor frame.  If set to null then (0,0,0) is assumed.
	 * @param InIncludeNonSimShapes True if all kind of shapes (Query shape, trigger shape) should be taken into account.
	 */
	static FORCEINLINE void UpdateMassAndInertia( const SPhysicsActorHandlePhysX& InActorHandle, float InDensity, const Vector* InMassLocalPose = nullptr, bool InIncludeNonSimShapes = false )
	{
		Assert( IsValidActor( InActorHandle ) );
		if ( InActorHandle.bStatic )
		{
			return;
		}

		physx::PxRigidBodyExt::updateMassAndInertia( *( physx::PxRigidDynamic* )InActorHandle.pxRigidActor, InDensity, InMassLocalPose ? &LE2PVector( *InMassLocalPose ) : nullptr, InIncludeNonSimShapes );
	}

	/**
	 * @brief Get physics actor transform
	 * 
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE CTransform GetTransform( const SPhysicsActorHandlePhysX& InActorHandle )
	{
		if ( !IsValidActor( InActorHandle ) )
		{
			return CTransform();
		}

		return P2LETransform( InActorHandle.pxRigidActor->getGlobalPose() );
	}

	/**
	 * @brief Release actor
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE void ReleaseActor( SPhysicsActorHandlePhysX& InActorHandle )
	{
		if ( InActorHandle.pxRigidActor )
		{
			InActorHandle.pxRigidActor->release();
			InActorHandle.pxRigidActor = nullptr;
		}
	}

	/**
	 * @brief Is valid actor
	 *
	 * @param InActorHandle Handle of physics actor
	 * @return Return true if actor handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidActor( const SPhysicsActorHandlePhysX& InActorHandle )
	{
		return InActorHandle.pxRigidActor;
	}

	/**
	 * @brief Attach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static FORCEINLINE void AttachShape( const SPhysicsActorHandlePhysX& InActorHandle, const SPhysicsShapeHandlePhysX& InShapeHandle )
	{
		Assert( IsValidActor( InActorHandle ) && IsValidShapeGeometry( InShapeHandle ) );
		InActorHandle.pxRigidActor->attachShape( *InShapeHandle.pxShape );
	}

	/**
	 * @brief Detach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static FORCEINLINE void DetachShape( const SPhysicsActorHandlePhysX& InActorHandle, const SPhysicsShapeHandlePhysX& InShapeHandle )
	{
		Assert( IsValidActor( InActorHandle ) && IsValidShapeGeometry( InShapeHandle ) );
		InActorHandle.pxRigidActor->detachShape( *InShapeHandle.pxShape );
	}
};
#endif // WITH_PHYSX

#endif // !PHYSICSINTERFACEPHYSX_H