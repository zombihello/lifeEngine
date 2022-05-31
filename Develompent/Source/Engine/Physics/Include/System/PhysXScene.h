/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSXSSCENE_H
#define PHYSXSSCENE_H

#if WITH_PHYSX
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include <vector>

/**
 * @ingroup Physics
 * @brief Class of PhysX scene
 */
class CPhysXScene
{
public:
	/**
	 * @brief Constructor
	 */
	CPhysXScene();

	/**
	 * @brief Destructor
	 */
	~CPhysXScene();

	/**
	 * @brief Initialize scene
	 */
	void Init();

	/**
	 * @brief Tick scene
	 * 
	 * @param InDeltaTime The time since the last tick
	 */
	void Tick( float InDeltaTime );

	/**
	 * @brief Shutdown scene
	 */
	void Shutdown();

	/**
	 * @brief Add body on scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void AddBody( class CPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove body from scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void RemoveBody( class CPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove all bodies from scene
	 */
	void RemoveAllBodies();

private:
	physx::PxScene*									pxScene;						/**< PhysX scene */
	physx::PxDefaultCpuDispatcher*					pxDefaultCpuDispatcher;			/**< Default CPU dispatcher */
	std::vector< class CPhysicsBodyInstance* >		bodies;							/**< Array of bodies on scene */
};
#endif // WITH_PHYSX

#endif // !PHYSXSSCENE_H