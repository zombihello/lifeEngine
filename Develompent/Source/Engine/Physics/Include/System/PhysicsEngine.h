/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSENGINE_H
#define PHYSICSENGINE_H

#include "System/PhysicsMaterial.h"
#include "Core.h"

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
	 * 
	 * @param InDeltaTime The time since the last tick
	 */
	void Tick( float InDeltaTime );

	/**
	 * @brief Shutdown engine
	 */
	void Shutdown();

	/**
	 * @brief Get default physics material
	 * @return Return default physics material
	 */
	FORCEINLINE FPhysicsMaterialRef GetDefaultPhysMaterial() const
	{
		return defaultPhysMaterial;
	}

private:
	FPhysicsMaterialRef		defaultPhysMaterial;			/**< Default physics material */
};

#endif // !PHYSICSENGINE_H