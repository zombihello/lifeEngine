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

#include "Math/Math.h"
#include "Misc/EngineTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Archive.h"
#include "PhysicsInterface.h"

/**
 * @ingroup Engine
 * The World is the top level object representing a map or a sandbox in which Actors and Components will exist and be rendered.
 */
class CWorld
{
public:
	/**
	 * Constructor
	 */
	CWorld();

	/**
	 * Destructor
	 */
	~CWorld();

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
	 * Serialize world
	 * 
	 * @param[in] InArchive Archive
	 */
	void Serialize( CArchive& InArchive );

	/**
	 * Clean up all world
	 */
	void CleanupWorld();

	/**
	 * Spawn actor in world
	 * 
	 * @param[in] InClass Class of spawned actor
	 * @param[in] InLocation Location actor on spawn
	 * @param[in] InRotation Rotation actor on spawn
	 */
	ActorRef_t SpawnActor( class CClass* InClass, const Vector& InLocation, const CRotator& InRotation = SMath::rotatorZero );

	/**
	 * Spawn actor in world
	 * 
	 * @param[in] InLocation Location actor on spawn
	 * @param[in] InRotation Rotation actor on spawn
	 */
	template< typename TClass >
	FORCEINLINE TRefCountPtr< TClass > SpawnActor( const Vector& InLocation, const CRotator& InRotation = SMath::rotatorZero )
	{
		return SpawnActor( TClass::StaticClass(), InLocation, InRotation );
	}

	/**
	 * Trace a ray against the world using a specific channel and return the first blocking hit
	 * 
	 * @param OutHitResult Hit result
	 * @param InStart Start ray
	 * @param InEnd End ray
	 * @param InTraceChannel Trace channel
	 * @param InCollisionQueryParams Collision query params
	 * @return Return TRUE if a blocking hit is found, else false
	 */
	FORCEINLINE bool LineTraceSingleByChannel( SHitResult& OutHitResult, const Vector& InStart, const Vector& InEnd, ECollisionChannel InTraceChannel, const SCollisionQueryParams& InCollisionQueryParams = SCollisionQueryParams::defaultQueryParam )
	{
		return GPhysicsScene.LineTraceSingleByChannel( OutHitResult, InStart, InEnd, InTraceChannel, InCollisionQueryParams );
	}

	/**
	 * @brief Get scene manager
	 * @return Return pointer to scene manager
	 */
	FORCEINLINE class CBaseScene* GetScene() const
	{
		return scene;
	}

private:
	bool							isBeginPlay;	/**< Is started gameplay */
	class CBaseScene*				scene;			/**< Scene manager */
	std::vector< ActorRef_t >		actors;			/**< Array actors in world */
};

#endif // !WORLD_H