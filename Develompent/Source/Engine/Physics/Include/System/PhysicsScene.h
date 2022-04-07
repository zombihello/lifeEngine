/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSSCENE_H
#define PHYSICSSCENE_H

#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <vector>

#include "System/PhysicsBodyInstance.h"

/**
 * @ingroup Physics
 * @brief Class of physics scene
 */
class FPhysicsScene
{
public:
	/**
	 * @brief Constructor
	 */
	FPhysicsScene();

	/**
	 * @brief Destructor
	 */
	~FPhysicsScene();

	/**
	 * @brief Initialize scene
	 */
	void Init();

	/**
	 * @brief Tick scene
	 */
	void Tick();

	/**
	 * @brief Shutdown scene
	 */
	void Shutdown();

	/**
	 * @brief Add body on scene
	 * @param InBodyInstance Pointer to body instance
	 */
	FORCEINLINE void AddBody( FPhysicsBodyInstance* InBodyInstance )
	{
		physx::PxRigidActor*		pxRigidBody = InBodyInstance->GetPhysXRigidBody();
		check( pxRigidBody );

		pxScene->addActor( *pxRigidBody );
		bodies.push_back( InBodyInstance );
	}

	/**
	 * @brief Remove body from scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void RemoveBody( FPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove all bodies from scene
	 */
	void RemoveAllBodies();

private:
	physx::PxScene*								pxScene;						/**< PhysX scene */
	physx::PxDefaultCpuDispatcher*				pxDefaultCpuDispatcher;			/**< Default CPU dispatcher */
	std::vector< FPhysicsBodyInstance* >		bodies;							/**< Array of bodies on scene */
};

#endif // !PHYSICSSCENE_H