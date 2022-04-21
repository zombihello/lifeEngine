/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSBODYINSTANCE_H
#define PHYSICSBODYINSTANCE_H

#include "System/PhysicsEngine.h"
#include "System/PhysicsBodySetup.h"

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
	 * @brief Add angular impulse
	 *
	 * @param InAngularImpulse Angular impulse
	 * @param InIsWake Also wake up the body
	 */
	FORCEINLINE void AddAngularImpulse( const FVector& InAngularImpulse, bool InIsWake )
	{
		FPhysicsInterface::AddAngularImpulse( handle, InAngularImpulse, InIsWake );
	}

	/**
	 * @brief Add impulse
	 *
	 * @param InImpulse Impulse
	 * @param InIsWake Also wake up the body
	 */
	FORCEINLINE void AddImpulse( const FVector& InImpulse, bool InIsWake )
	{
		FPhysicsInterface::AddImpulse( handle, InImpulse, InIsWake );
	}

	/**
	 * @brief Add impulse at location
	 *
	 * @param InImpulse Impulse
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	FORCEINLINE void AddImpulseAtLocation( const FVector& InImpulse, const FVector& InLocation, bool InIsWake )
	{
		FPhysicsInterface::AddImpulseAtLocation( handle, InImpulse, InLocation, InIsWake );
	}

	/**
	 * @brief Add force
	 *
	 * @param InForce Force
	 * @param InIsWake Also wake up the body
	 */
	FORCEINLINE void AddForce( const FVector& InForce, bool InIsWake )
	{
		FPhysicsInterface::AddForce( handle, InForce, InIsWake );
	}

	/**
	 * @brief Add force at location
	 *
	 * @param InForce Force
	 * @param InLocation Location
	 * @param InIsWake Also wake up the body
	 */
	FORCEINLINE void AddForceAtLocation( const FVector& InForce, const FVector& InLocation, bool InIsWake )
	{
		FPhysicsInterface::AddForceAtLocation( handle, InForce, InLocation, InIsWake );
	}

	/**
	 * @brief Set linear velocity
	 *
	 * @param InVelocity New linear velocity to apply to physics
	 * @param InIsAddToCurrent If true, InVelocity is added to the existing velocity of the body
	 */
	FORCEINLINE void SetLinearVelocity( const FVector& InVelocity, bool InIsAddToCurrent = false )
	{
		FPhysicsInterface::SetLinearVelocity( handle, InVelocity, InIsAddToCurrent );
	}

	/**
	 * @brief Set dynamic body
	 * 
	 * @param InIsDynamic Is dynamic body
	 * @warning After call this method need call InitBody for reinit body
	 */
	FORCEINLINE void SetDynamic( bool InIsDynamic )
	{
		bDirty = !bStatic != InIsDynamic ? true : bDirty;
		bStatic = !InIsDynamic;
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
	 * @brief Set enable gravity
	 * @param InEnableGravity Is need enable gravity for body
	 */
	FORCEINLINE void SetEnableGravity( bool InEnableGravity )
	{
		bEnableGravity = InEnableGravity;
	}

	/**
	 * @brief Set simulate physics
	 * @param InSimulatePhysics Is need simulate physics for body
	 */
	FORCEINLINE void SetSimulatePhysics( bool InSimulatePhysics )
	{
		bDirty = bSimulatePhysics != InSimulatePhysics ? true : bDirty;
		bSimulatePhysics = InSimulatePhysics;
	}

	/**
	 * @brief Set body awake on start
	 * @param InStartAwake Is need awake body on start
	 */
	FORCEINLINE void SetStartAwake( bool InStartAwake )
	{
		bStartAwake = InStartAwake;
	}

	/**
	 * @brief Is dynamic rigid body
	 * @return Returns true if the body is not static
	 */
 	FORCEINLINE bool IsDynamic() const
	{
		return !bStatic;
	}

	/**
	 * @brief Get LE world transform
	 * @return Return LE world transform
	 */
	FORCEINLINE FTransform GetLEWorldTransform() const
	{
		return FPhysicsInterface::GetTransform( handle );
	}

	/**
	 * @brief Get body setup
	 * @return Return body setup
	 */
	FORCEINLINE FPhysicsBodySetupRef GetBodySetup() const
	{
		return bodySetup;
	}

	/**
	 * @brief Get linear velocity
	 * @return Return actor linear velocity
	 */
	FORCEINLINE FVector GetLinearVelocity()
	{
		return FPhysicsInterface::GetLinearVelocity( handle );
	}

	/**
	 * @brief Is valid body instance
	 * @return Return true if body instance is valid, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return FPhysicsInterface::IsValidActor( handle );
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

	/**
	 * @brief Get handle to physics actor
	 * @return Return physics actor handle
	 */
	FORCEINLINE FPhysicsActorHandle GetActorHandle() const
	{
		return handle;
	}

	/**
	 * @brief Get owner primitive component
	 * @return Return pointer to owner primitive component, if not exist return NULL
	 */
	FORCEINLINE TRefCountPtr< class LPrimitiveComponent > GetOwnerComponent() const
	{
		return ownerComponent;
	}

	/**
	 * @brief Is enabled gravity
	 * @return Return true if for body enabled gravity, else return false
	 */
	FORCEINLINE bool IsEnableGravity() const
	{
		return bEnableGravity;
	}

	/**
	 * @brief Is simulate physics
	 * @return Return true if for body enabled simulate physics, else return false
	 */
	FORCEINLINE bool IsSimulatePhysics() const
	{
		return bSimulatePhysics;
	}

	/**
	 * @brief Is body awake on start
	 * @return Return true if body awake on start, else return false
	 */
	FORCEINLINE bool IsStartAwake() const
	{
		return bStartAwake;
	}

	/**
	 * @brief Is body need reinit
	 * @return Return true if body is ditrty, else return false
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
	}

private:
	bool											bStatic;			/**< Is static rigid body */
	bool											bEnableGravity;		/**< Enable gravity */
	bool											bSimulatePhysics;	/**< Need simulate physics */
	bool											bStartAwake;		/**< Start awake */
	bool											bDirty;				/**< Is body is dirty and need reinit hem */
	uint32											lockFlags;			/**< Lock flags */
	float											mass;				/**< Mass of body */
	TRefCountPtr< class LPrimitiveComponent >		ownerComponent;		/**< PrimitiveComponent containing this body */	
	FPhysicsBodySetupRef							bodySetup;			/**< Body setup */
	FPhysicsActorHandle								handle;				/**< Handle to physics actor */
};

#endif // !PHYSICSBODYINSTANCE_H