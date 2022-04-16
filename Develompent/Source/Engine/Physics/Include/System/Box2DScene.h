/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOX2DSSCENE_H
#define BOX2DSSCENE_H

#if WITH_BOX2D
#include <box2d/box2d.h>
#include <vector>

#include "CoreDefines.h"

/**
 * @ingroup Physics
 * @brief Class of PhysX scene
 */
class FBox2DScene
{
public:
	/**
	 * @brief Constructor
	 */
	FBox2DScene();

	/**
	 * @brief Destructor
	 */
	~FBox2DScene();

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
	FORCEINLINE void AddBody( class FPhysicsBodyInstance* InBodyInstance )
	{
		bodies.push_back( InBodyInstance );
	}

	/**
	 * @brief Remove body from scene
	 * @param InBodyInstance Pointer to body instance
	 */
	void RemoveBody( class FPhysicsBodyInstance* InBodyInstance );

	/**
	 * @brief Remove all bodies from scene
	 */
	FORCEINLINE void RemoveAllBodies()
	{
		bodies.clear();
	}

	/**
	 * @brief Get Box2D world
	 * @return Return Box2D world
	 */
	FORCEINLINE b2World* GetBox2DWorld() const
	{
		return bx2World;
	}

private:
	b2World*										bx2World;						/**< Box2D world */
	std::vector< class FPhysicsBodyInstance* >		bodies;							/**< Array of bodies on scene */
};
#endif // WITH_BOX2D

#endif // !BOX2DSSCENE_H