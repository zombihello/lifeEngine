/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLD_H
#define WORLD_H

#include <vector>

#include "Misc/EngineTypes.h"

/**
 * @ingroup Engine
 * The World is the top level object representing a map or a sandbox in which Actors and Components will exist and be rendered.
 */
class FWorld
{
public:
	/**
	 * Constructor
	 */
	FWorld();

	/**
	 * Destructor
	 */
	~FWorld();

	/**
	 * Start gameplay. This will cause the game mode to transition to the correct state and call BeginPlay on all actors
	 */
	void BeginPlay();

	/**
	 * Update world
	 * 
	 * @param[in] InDeltaTime The time since the last tick
	 */
	void Tick( float InDeltaTime );

	/**
	 * Clean up all world
	 */
	void CleanupWorld();

	/**
	 * Spawn actor in world
	 * 
	 * @param[in] InClass Class of spawned actor
	 */
	AActorRef SpawnActor( class LClass* InClass );

	/**
	 * Spawn actor in world
	 */
	template< typename TClass >
	TRefCountPtr< TClass > SpawnActor()
	{
		return SpawnActor( TClass::StaticClass() );
	}

private:
	bool							isBeginPlay;	/**< Is started gameplay */
	std::vector< AActorRef >		actors;			/**< Array actors in world */
};

#endif // !WORLD_H