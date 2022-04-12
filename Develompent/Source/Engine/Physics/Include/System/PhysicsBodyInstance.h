/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSBODYINSTANCE_H
#define PHYSICSBODYINSTANCE_H

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

#include "System/PhysicsEngine.h"
#include "System/PhysicsBodySetup.h"

/**
 * @ingroup Physics
 * @brief Enumeration of rigid body lock flag
 */
enum EBodyLockFlag
{
	BLF_None		= 0,		/**< Nope flag */
	BLF_LockMoveX	= 1 << 0,	/**< Lock moving by X axis */
	BLF_LockMoveY	= 1 << 1,	/**< Lock moving by Y axis */
	BLF_LockMoveZ	= 1 << 2,	/**< Lock moving by Z axis */
	BLF_LockRotateX	= 1 << 3,	/**< Lock rotating by X axis */
	BLF_LockRotateY	= 1 << 4,	/**< Lock rotating by Y axis */
	BLF_LockRotateZ	= 1 << 5	/**< Lock rotating by Z axis */
};

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
 * @brief Container for a physics representation of an object
 */
class FPhysicsBodyInstance
{
public:
	/**
	 * @brief Constructor
	 */
	FPhysicsBodyInstance();

	/**
	 * @brief Destructor
	 */
	~FPhysicsBodyInstance();

	/**
	 * @brief Initialize rigid body
	 * 
	 * @param InBodySetup Rigid body setup
	 * @param InTransform Initial transform of body
	 * @param InPrimComp PrimitiveComponent containing this body
	 */
	void InitBody( FPhysicsBodySetup* InBodySetup, const FTransform& InTransform, class LPrimitiveComponent* InPrimComp );

	/**
	 * @brief Terminates the body, releasing resources
	 */
	void TermBody();

	/**
	 * @brief Set dynamic body
	 * 
	 * @param InIsDynamic Is dynamic body
	 * @warning After call this method need call InitBody for reinit body
	 */
	FORCEINLINE void SetDynamic( bool InIsDynamic )
	{
		bIsStatic = !InIsDynamic;
	}

	/**
	 * @brief Set lock flags
	 *
	 * @param InLockFlags Lock flags
	 */
	FORCEINLINE void SetLockFlags( uint32 InLockFlags )
	{
		lockFlags = InLockFlags;
	}

	/**
	 * @brief Set body mass
	 * @param InMass Mass
	 */
	FORCEINLINE void SetMass( float InMass )
	{
		mass = InMass;
	}

	/**
	 * @brief Is dynamic rigid body
	 * @return Returns true if the body is not static
	 */
 	FORCEINLINE bool IsDynamic() const
	{
		return !bIsStatic;
	}

	/**
	 * @brief Get LE world transform
	 * @return Return LE world transform
	 */
	FORCEINLINE FTransform GetLEWorldTransform() const
	{
		return pxRigidBody ? P2LETransform( pxRigidBody->getGlobalPose() ) : FTransform();
	}

	/**
	 * @brief Get PhysX rigid body
	 * @return Return PhysX rigid body. Is not inited return nullptr
	 */
	FORCEINLINE physx::PxRigidActor* GetPhysXRigidBody() const
	{
		return pxRigidBody;
	}

	/**
	 * @brief Is valid body instance
	 * @return Return true if body instance is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return pxRigidBody;
	}

	/**
	 * @brief Get lock flags
	 * @return Return lock flags of body
	 */
	FORCEINLINE uint32 GetLockFlags() const
	{
		return lockFlags;
	}

	/**
	 * @brief Get body mass
	 * @return Return body mass
	 */
	FORCEINLINE float GetMass() const
	{
		return mass;
	}

private:
	bool											bIsStatic;			/**< Is static rigid body */
	uint32											lockFlags;			/**< Lock flags */
	float											mass;				/**< Mass of body */
	TRefCountPtr< class LPrimitiveComponent >		ownerComponent;		/**< PrimitiveComponent containing this body */	
	FPhysicsBodySetupRef							bodySetup;			/**< Body setup */
	physx::PxRigidActor*							pxRigidBody;		/**< PhysX rigid body */
};

#endif // !PHYSICSBODYINSTANCE_H