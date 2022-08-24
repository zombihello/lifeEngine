/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSINTERFACEBOX2D_H
#define PHYSICSINTERFACEBOX2D_H

#if WITH_BOX2D
#include <box2d/box2d.h>
#include <unordered_map>

#include "Math/Math.h"
#include "Math/Transform.h"
#include "Misc/SharedPointer.h"
#include "Misc/Box2DGlobals.h"
#include "System/PhysicsMaterialDelegates.h"
#include "System/Box2DScene.h"

/**
 * @ingroup Physics
 * @brief Box2D scale world (in Box2D all in meters)
 */
#define BOX2D_SCALE			30.f

/**
 * @ingroup Physics
 * @brief Box2D angles (for convert radians to degrees)
 */
#define BOX2D_ANGLES		57.29577f

/**
 * @ingroup Physics
 * @brief Convert LE CRotator to Box2D b2Rot
 *
 * @param InRot LE rotator
 */
FORCEINLINE b2Rot LE2BRot( const Quaternion& InRot )
{
	return b2Rot( SMath::QuaternionToAngles( InRot ).y / BOX2D_ANGLES );
}

/**
 * @ingroup Physics
 * @brief Convert LE Vector to Box2D b2Vec3
 *
 * @param InVec LE vector
 */
FORCEINLINE b2Vec3 LE2BVector( const Vector& InVec )
{
	return b2Vec3( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert LE Vector2D to Box2D b2Vec2
 *
 * @param InVec LE vector 2D
 */
FORCEINLINE b2Vec2 LE2BVector( const Vector2D& InVec )
{
	return b2Vec2( InVec.x, InVec.y );
}

/**
 * @ingroup Physics
 * @brief Convert LE CTransform to Box2D b2Transform
 *
 * @param InTransform LE transform
 */
FORCEINLINE b2Transform LE2BTransform( const CTransform& InTransform )
{
	b2Rot			bx2Rot	= LE2BRot( InTransform.GetRotation() );
	b2Vec2			bx2Pos	= LE2BVector( ( Vector2D )InTransform.GetLocation() / BOX2D_SCALE );
	b2Transform		result( bx2Pos, bx2Rot );
	return result;
}

/**
 * @ingroup Physics
 * @brief Convert Box2D b2Rot to LE CRotator
 *
 * @param InRot Box2D rotator
 */
FORCEINLINE Quaternion B2LERot( const b2Rot& InRot )
{
	return SMath::AnglesToQuaternionXYZ( Vector( 0.f, 0.f, InRot.GetAngle() * BOX2D_ANGLES ) );
}

/**
 * @ingroup Physics
 * @brief Convert Box2D b2Vec3 to LE Vector
 *
 * @param InVec Box2D vector
 */
FORCEINLINE Vector B2LEVector( const b2Vec3& InVec )
{
	return Vector( InVec.x, InVec.y, InVec.z );
}

/**
 * @ingroup Physics
 * @brief Convert Box2D b2Vec2 to LE Vector2D
 *
 * @param InVec Box2D vector 2D
 */
FORCEINLINE Vector2D B2LEVector( const b2Vec2& InVec )
{
	return Vector2D( InVec.x, InVec.y );
}

/**
 * @ingroup Physics
 * @brief Convert Box2D b2Transform to LE CTransform
 *
 * @param InTransform Box2D transform
 */
FORCEINLINE CTransform B2LETransform( const b2Transform& InTransform )
{
	Quaternion		leRot = B2LERot( InTransform.q );
	Vector2D		lePos = B2LEVector( InTransform.p ) * BOX2D_SCALE;

	CTransform		result( leRot, Vector( lePos, 0.f ) );
	return result;
}

/**
 * @ingroup Physics
 * @brief Convert lock flags from LE to Box2D
 */
FORCEINLINE bool LE2BLockFlags( uint32 InLockFlags )
{
	// Lock rotating by Z
	if ( InLockFlags & BLF_LockRotateZ )
	{
		return true;
	}

	return false;
}

/**
 * @ingroup Physics
 * @brief Handle of material for Box2D
 */
struct SPhysicsMaterialHandleBox2D
{
	TAssetHandle<class CPhysicsMaterial>	physMaterial;				/**< Physical material */
};

/**
 * @ingroup Physics
 * @brief Handle of shape for Box2D
 */
struct SPhysicsShapeHandleBox2D
{
	/**
	 * @brief Constructor
	 */
	SPhysicsShapeHandleBox2D()
		: collisionProfile( nullptr )
		, bx2Shape( nullptr )
	{}

	TAssetHandle<class CPhysicsMaterial>	physMaterial;		/**< Physical material */
	SCollisionProfile*						collisionProfile;	/**< Collision profile */
	b2Shape*								bx2Shape;			/**< Box2D shape */
};

/**
 * @ingroup Physics
 * @brief Handle of actor for Box2D
 */
struct SPhysicsActorHandleBox2D
{
	/**
	 * @brief Constructor
	 */
	SPhysicsActorHandleBox2D()
		: bx2Body( nullptr )
		, physicsMaterialUpdateHandle( nullptr )
		, physicsMaterialDestroyedHandle( nullptr )
	{}

	/**
	 * @brief On material updated
	 */
	void OnMaterialUpdated( const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial );

	/**
	 * @brief On material destroyed
	 */
	void OnMaterialDestroyed( const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial );

	b2Body*											bx2Body;						/**< Box2D rigid body */
	std::unordered_map< b2Shape*, b2Fixture* >		fixtureMap;						/**< Fixture map */
	COnPhysicsMaterialUpdate::DelegateType_t*		physicsMaterialUpdateHandle;	/**< Handle delegate of physics material is updated */
	COnPhysicsMaterialDestroyed::DelegateType_t*	physicsMaterialDestroyedHandle;	/**< Handle delegate of physics material is destroyed */
};

/**
 * @ingroup Physics
 * @brief Box2D interface
 */
struct SPhysicsInterfaceBox2D
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
	 * @param InBoxGeometry Box geometry info
	 * @return Return shape handle
	 */
	static SPhysicsShapeHandleBox2D CreateShapeGeometry( const struct SPhysicsBoxGeometry& InBoxGeometry );

	/**
	 * @brief Release shape
	 * @param InShapeHandle Shape handle
	 */
	static FORCEINLINE void ReleaseShapeGeometry( SPhysicsShapeHandleBox2D& InShapeHandle )
	{
		if ( InShapeHandle.bx2Shape )
		{
			delete InShapeHandle.bx2Shape;
			InShapeHandle.bx2Shape = nullptr;
		}
		InShapeHandle.physMaterial = nullptr;
	}

	/**
	 * @brief Is valid shape
	 *
	 * @param InShapeHandle Shape handle
	 * @return Return true if shape handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidShapeGeometry( const SPhysicsShapeHandleBox2D& InShapeHandle )
	{
		return InShapeHandle.physMaterial.IsAssetValid() && InShapeHandle.bx2Shape;
	}

	/**
	 * @brief Create material
	 *
	 * @param InPhysMaterial Physics material
	 * @return Return material handle
	 */
	static SPhysicsMaterialHandleBox2D CreateMaterial( const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial );

	/**
	 * @brief Update material
	 *
	 * @param InMaterialHandle Handle of phys material
	 * @param InPhysMaterial Physics material
	 */
	static FORCEINLINE void UpdateMaterial( const SPhysicsMaterialHandleBox2D& InMaterialHandle, const TSharedPtr<class CPhysicsMaterial>& InPhysMaterial )
	{}

	/**
	 * @brief Release material
	 * @param InMaterialHandle Handle of phys material
	 */
	static FORCEINLINE void ReleaseMaterial( SPhysicsMaterialHandleBox2D& InMaterialHandle )
	{
		InMaterialHandle.physMaterial = nullptr;
	}

	/**
	 * @brief Is valid material
	 *
	 * @param InMaterialHandle Material handle
	 * @return Return true if material handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidMaterial( const SPhysicsMaterialHandleBox2D& InMaterialHandle )
	{
		return InMaterialHandle.physMaterial.IsAssetValid();
	}

	/**
	 * @brief Create actor
	 *
	 * @param InParams Actor params for create
	 * @return Return actor handle
	 */
	static SPhysicsActorHandleBox2D CreateActor( const SActorCreationParams& InParams );

	/**
	 * @brief Computation of mass properties for a physics actor
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InDensity The density of the body. Used to compute the mass of the body. The density must be greater than 0.
	 * @param InMassLocalPose The center of mass relative to the actor frame.  If set to null then (0,0,0) is assumed.
	 * @param InIncludeNonSimShapes True if all kind of shapes (Query shape, trigger shape) should be taken into account.
	 */
	static FORCEINLINE void UpdateMassAndInertia( const SPhysicsActorHandleBox2D& InActorHandle, float InDensity, const Vector* InMassLocalPose = nullptr, bool InIncludeNonSimShapes = false )
	{}

	/**
	 * @brief Get physics actor transform
	 *
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE CTransform GetTransform( const SPhysicsActorHandleBox2D& InActorHandle )
	{
		if ( !IsValidActor( InActorHandle ) )
		{
			return CTransform();
		}

		return B2LETransform( InActorHandle.bx2Body->GetTransform() );
	}

	/**
	 * @brief Set linear velocity
	 * 
	 * @param InActorHandle Handle of physics actor
	 * @param InVelocity New linear velocity to apply to physics
	 * @param InIsAddToCurrent If true, InVelocity is added to the existing velocity of the body
	 */
	static FORCEINLINE void SetLinearVelocity( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InVelocity, bool InIsAddToCurrent = false )
	{
		check( IsValidActor( InActorHandle ) );
		b2Vec2			bx2NewVelocity = LE2BVector( ( Vector2D )InVelocity / BOX2D_SCALE );
		InActorHandle.bx2Body->SetLinearVelocity( !InIsAddToCurrent ? bx2NewVelocity : InActorHandle.bx2Body->GetLinearVelocity() + bx2NewVelocity );
	}

	/**
	 * @brief Get linear velocity
	 * 
	 * @param InActorHandle Handle of physics actor
	 * @return Return actor linear velocity
	 */
	static FORCEINLINE Vector GetLinearVelocity( const SPhysicsActorHandleBox2D& InActorHandle )
	{
		check( IsValidActor( InActorHandle ) );
		return Vector( B2LEVector( InActorHandle.bx2Body->GetLinearVelocity() ) * BOX2D_SCALE, 0.f );
	}

	/**
	 * @brief Add angular impulse
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InAngularImpulse Angular impulse
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddAngularImpulse( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InAngularImpulse, bool InIsWake )
	{
		check( IsValidActor( InActorHandle ) );
		InActorHandle.bx2Body->ApplyAngularImpulse( InAngularImpulse.z / BOX2D_ANGLES, InIsWake );
	}

	/**
	 * @brief Add impulse
	 * 
	 * @param InActorHandle Handle of physics actor
	 * @param InImpulse Impulse
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddImpulse( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InImpulse, bool InIsWake )
	{
		check( IsValidActor( InActorHandle ) );
		InActorHandle.bx2Body->ApplyLinearImpulse( LE2BVector( ( Vector2D )InImpulse ), b2Vec2( 0.f, 0.f ), InIsWake );
	}

	/**
	 * @brief Add impulse at location
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InImpulse Impulse
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddImpulseAtLocation( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InImpulse, const Vector& InLocation, bool InIsWake )
	{
		check( IsValidActor( InActorHandle ) );
		InActorHandle.bx2Body->ApplyLinearImpulse( LE2BVector( ( Vector2D )InImpulse ), LE2BVector( ( Vector2D )InLocation / BOX2D_SCALE ), InIsWake );
	}

	/**
	 * @brief Add force
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InForce Force
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddForce( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InForce, bool InIsWake )
	{
		check( IsValidActor( InActorHandle ) );
		InActorHandle.bx2Body->ApplyForceToCenter( LE2BVector( ( Vector2D )InForce ), InIsWake );
	}

	/**
	 * @brief Add force at location
	 *
	 * @param InActorHandle Handle of physics actor
	 * @param InForce Force
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	static FORCEINLINE void AddForceAtLocation( const SPhysicsActorHandleBox2D& InActorHandle, const Vector& InForce, const Vector& InLocation, bool InIsWake )
	{
		check( IsValidActor( InActorHandle ) );
		InActorHandle.bx2Body->ApplyForce( LE2BVector( ( Vector2D )InForce ), LE2BVector( ( Vector2D )InLocation / BOX2D_SCALE ), InIsWake );
	}

	/**
	 * @brief Release actor
	 * @param InActorHandle Handle of physics actor
	 */
	static FORCEINLINE void ReleaseActor( SPhysicsActorHandleBox2D& InActorHandle )
	{
		if ( InActorHandle.bx2Body )
		{
			GPhysicsScene.GetBox2DWorld()->DestroyBody( InActorHandle.bx2Body );
			InActorHandle.bx2Body = nullptr;
		}
	}

	/**
	 * @brief Is valid actor
	 *
	 * @param InActorHandle Handle of physics actor
	 * @return Return true if actor handle is valid, else return false
	 */
	static FORCEINLINE bool IsValidActor( const SPhysicsActorHandleBox2D& InActorHandle )
	{
		return InActorHandle.bx2Body;
	}

	/**
	 * @brief Attach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static void AttachShape( SPhysicsActorHandleBox2D& InActorHandle, const SPhysicsShapeHandleBox2D& InShapeHandle );

	/**
	 * @brief Detach shape
	 * @param InActorHandle Handle of physics actor
	 * @param InShapeHandle Handle of physics shape
	 */
	static void DetachShape( SPhysicsActorHandleBox2D& InActorHandle, const SPhysicsShapeHandleBox2D& InShapeHandle );
};
#endif // WITH_BOX2D

#endif // !PHYSICSINTERFACEBOX2D_H