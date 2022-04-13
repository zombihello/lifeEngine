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
		return FPhysicsInterface::GetTransform( handle );
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

private:
	bool											bIsStatic;			/**< Is static rigid body */
	uint32											lockFlags;			/**< Lock flags */
	float											mass;				/**< Mass of body */
	TRefCountPtr< class LPrimitiveComponent >		ownerComponent;		/**< PrimitiveComponent containing this body */	
	FPhysicsBodySetupRef							bodySetup;			/**< Body setup */
	FPhysicsActorHandle								handle;				/**< Handle to physics actor */
};

#endif // !PHYSICSBODYINSTANCE_H