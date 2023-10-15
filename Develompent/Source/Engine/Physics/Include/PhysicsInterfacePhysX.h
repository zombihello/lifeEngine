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
	//physx::PxQuat	pxQuat = LE2PQuat( InTransform.GetRotation().ToQuaternion() );
	//physx::PxVec3	pxPos = LE2PVector( InTransform.GetLocation() );
	return physx::PxTransform();
	//physx::PxTransform		result( pxPos, pxQuat );
	//return result;
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
//FORCEINLINE CTransform P2LETransform( const physx::PxTransform& InTransform )
//{
//	Quaternion		leQuat = P2LEQuat( InTransform.q );
//	Vector			lePos = P2LEVector( InTransform.p );
//
//	CTransform		result( CRotator( leQuat ), lePos );
//	return result;
//}

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
struct PhysicsMaterialHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	PhysicsMaterialHandlePhysX()
		: pxMaterial( nullptr )
	{}

	physx::PxMaterial*		pxMaterial;		/**< PhysX material */
};

/**
 * @ingroup Physics
 * @brief Handle of shape for PhysX
 */
struct PhysicsShapeHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	PhysicsShapeHandlePhysX()
		: pxShape( nullptr )
	{}

	physx::PxShape*		pxShape;		/**< PhysX shape */
};

/**
 * @ingroup Physics
 * @brief Handle of actor for PhysX
 */
struct PhysicsActorHandlePhysX
{
	/**
	 * @brief Constructor
	 */
	PhysicsActorHandlePhysX()
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
struct PhysicsInterfacePhysX
{
	static void Init();

	/**
	 * @brief Shutdown physics
	 */
	static void Shutdown();

	/**
	 * @brief Create box shape
	 *
	 * @param InBoxGeometry Box geometry info
	 * @return Return shape handle
	 */
	static PhysicsShapeHandlePhysX CreateShapeGeometry(const struct PhysicsBoxGeometry& InBoxGeometry);

	/**
	 * @brief Release shape
	 * @param InShapeHandle Shape handle
	 */
	static FORCEINLINE void ReleaseShapeGeometry(PhysicsShapeHandlePhysX& InShapeHandle)
	{
		//if (InShapeHandle.bx2Shape)
		//{
		//	delete InShapeHandle.bx2Shape;
		//	InShapeHandle.bx2Shape = nullptr;
		//}
		//InShapeHandle.physMaterial = nullptr;
	}

	/**
	 * @brief Is valid shape
	 *
	 * @param InShapeHandle Shape handle
	 * @return Return true if shape handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidShapeGeometry(const PhysicsShapeHandlePhysX& InShapeHandle)
	{
		return true;
		/*return InShapeHandle.physMaterial.IsAssetValid() && InShapeHandle.bx2Shape;*/
	}

	/**
	 * @brief Create material
	 *
	 * @param InPhysMaterial Physics material
	 * @return Return material handle
	 */
	static PhysicsMaterialHandlePhysX CreateMaterial(const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial);

	/**
	 * @brief Update material
	 *
	 * @param InMaterialHandle Handle of phys material
	 * @param InPhysMaterial Physics material
	 */
	static FORCEINLINE void UpdateMaterial(const PhysicsMaterialHandlePhysX& InMaterialHandle, const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial)
	{}

	/**
	 * @brief Release material
	 * @param InMaterialHandle Handle of phys material
	 */
	static FORCEINLINE void ReleaseMaterial(PhysicsMaterialHandlePhysX& InMaterialHandle)
	{
		//InMaterialHandle.physMaterial = nullptr;
	}

	/**
	 * @brief Is valid material
	 *
	 * @param InMaterialHandle Material handle
	 * @return Return true if material handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidMaterial(const PhysicsMaterialHandlePhysX& InMaterialHandle)
	{
		//return InMaterialHandle.physMaterial.IsAssetValid();
	}

	/**
	 * @brief Create actor
	 *
	 * @param InParams Actor params for create
	 * @return Return actor handle
	 */
	static PhysicsActorHandlePhysX CreateActor(const ActorCreationParams& InParams);

	/**
	 * @brief Computation of mass properties for a physics actor
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InDensity The density of the body. Used to compute the mass of the body. The density must be greater than 0.
	 * @param InMassLocalPose The center of mass relative to the actor frame.  If set to null then (0,0,0) is assumed.
	 * @param InIncludeNonSimShapes True if all kind of shapes (Query shape, trigger shape) should be taken into account.
	 */
	static FORCEINLINE void UpdateMassAndInertia(const PhysicsActorHandlePhysX& InActorHandle, float InDensity, const Vector* InMassLocalPose = nullptr, bool InIncludeNonSimShapes = false)
	{}

	/**
	 * @brief Get physics actor transform
	 *
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE CTransform GetTransform(const PhysicsActorHandlePhysX& InActorHandle)
	{
		//if (!IsValidActor(InActorHandle))
		//{
		//	return CTransform();
		//}

		//return B2LETransform(InActorHandle.bx2Body->GetTransform());
		return CTransform();
	}

	/**
	 * @brief Set linear velocity
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InVelocity New linear velocity to apply to physics
	 * @param InIsAddToCurrent If true, InVelocity is added to the existing velocity of the body
	 */
	static FORCEINLINE void SetLinearVelocity(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InVelocity, bool InIsAddToCurrent = false)
	{
		/*Assert(IsValidActor(InActorHandle));
		b2Vec2			bx2NewVelocity = LE2BVector((Vector2D)InVelocity / BOX2D_SCALE);
		InActorHandle.bx2Body->SetLinearVelocity(!InIsAddToCurrent ? bx2NewVelocity : InActorHandle.bx2Body->GetLinearVelocity() + bx2NewVelocity);*/
	}

	/**
	 * @brief Get linear velocity
	 *
	 * @param InActorHandle Handle of physics actor
	 * @return Return actor linear velocity
	 */
	static FORCEINLINE Vector GetLinearVelocity(const PhysicsActorHandlePhysX& InActorHandle)
	{
		//Assert(IsValidActor(InActorHandle));
		//return Vector(B2LEVector(InActorHandle.bx2Body->GetLinearVelocity()) * BOX2D_SCALE, 0.f);
		return Vector(0);
	}

	/**
	 * @brief Add angular impulse
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InAngularImpulse Angular impulse
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddAngularImpulse(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InAngularImpulse, bool InIsWake)
	{
		Assert(IsValidActor(InActorHandle));
		//InActorHandle.bx2Body->ApplyAngularImpulse(InAngularImpulse.z / BOX2D_ANGLES, InIsWake);
	}

	/**
	 * @brief Add impulse
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InImpulse Impulse
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddImpulse(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InImpulse, bool InIsWake)
	{
		//Assert(IsValidActor(InActorHandle));
		//InActorHandle.bx2Body->ApplyLinearImpulse(LE2BVector((Vector2D)InImpulse), b2Vec2(0.f, 0.f), InIsWake);
	}

	/**
	 * @brief Add impulse at location
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InImpulse Impulse
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddImpulseAtLocation(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InImpulse, const Vector& InLocation, bool InIsWake)
	{
		Assert(IsValidActor(InActorHandle));
		//InActorHandle.bx2Body->ApplyLinearImpulse(LE2BVector((Vector2D)InImpulse), LE2BVector((Vector2D)InLocation / BOX2D_SCALE), InIsWake);
	}

	/**
	 * @brief Add force
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InForce Force
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddForce(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InForce, bool InIsWake)
	{
		Assert(IsValidActor(InActorHandle));
		//InActorHandle.bx2Body->ApplyForceToCenter(LE2BVector((Vector2D)InForce), InIsWake);
	}

	/**
	 * @brief Add force at location
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InForce Force
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddForceAtLocation(const PhysicsActorHandlePhysX& InActorHandle, const Vector& InForce, const Vector& InLocation, bool InIsWake)
	{
		Assert(IsValidActor(InActorHandle));
		//InActorHandle.bx2Body->ApplyForce(LE2BVector((Vector2D)InForce), LE2BVector((Vector2D)InLocation / BOX2D_SCALE), InIsWake);
	}

	/**
	 * @brief Release actor
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE void ReleaseActor(PhysicsActorHandlePhysX& InActorHandle)
	{
		//if (InActorHandle.bx2Body)
		//{
		//	g_PhysicsScene.GetBox2DWorld()->DestroyBody(InActorHandle.bx2Body);
		//	InActorHandle.bx2Body = nullptr;
		//}
	}

	/**
	 * @brief Is valid actor
	 *
	 * @param InActorHandle Handle of physics actor
	 * @return Return true if actor handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidActor(const PhysicsActorHandlePhysX& InActorHandle)
	{
		return true; /*InActorHandle.bx2Body*/;
	}

	/**
	 * @brief Attach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static void AttachShape(PhysicsActorHandlePhysX& InActorHandle, const PhysicsShapeHandlePhysX& InShapeHandle);

	/**
	 * @brief Detach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static void DetachShape(PhysicsActorHandlePhysX& InActorHandle, const PhysicsShapeHandlePhysX& InShapeHandle);
};
#endif // WITH_PHYSX

#endif // !PHYSICSINTERFACEPHYSX_H