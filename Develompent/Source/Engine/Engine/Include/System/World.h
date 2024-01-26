/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLD_H
#define WORLD_H

#include <string>
#include <vector>

#include "Math/Math.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Archive.h"
#include "Actors/Actor.h"
#include "PhysicsInterface.h"

#if WITH_EDITOR
#include "WorldEd.h"
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * The World is the top level object representing a map or a sandbox in which Actors and Components will exist and be rendered.
 */
class CWorld : public CObject
{
	DECLARE_CLASS( CWorld, CObject, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	CWorld();

	/**
	 * @brief Called before destroying the object
	 * This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process
	 */
	virtual void BeginDestroy() override;

	/**
	 * @brief Do any object-specific cleanup required immediately after loading an object
	 * @note This is not called for newly-created objects, and by default will always execute on the game thread
	 */
	virtual void PostLoad() override;

	/**
	 * Start gameplay. This will cause the game mode to transition to the correct state and call BeginPlay on all actors
	 */
	void BeginPlay();

	/**
	 * End gameplay. This will cause the reset game mode and call EndPlay on all actors
	 */
	void EndPlay();

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
	virtual void Serialize( CArchive& InArchive ) override;

	/**
	 * Clean up all world
	 */
	void CleanupWorld();

	/**
	 * Spawn actor in world
	 * 
	 * @param InClass		Class of spawned actor
	 * @param InLocation	Location actor on spawn
	 * @param InRotation	Rotation actor on spawn
	 * @param InName		Actor name
	 */
	AActor* SpawnActor( class CClass* InClass, const Vector& InLocation, const CRotator& InRotation = Math::rotatorZero, const CName& InName = NAME_None );

	/**
	 * Destroy actor in world
	 * @param InActor	Actor
	 */
	FORCEINLINE void DestroyActor( AActor* InActor )
	{
		DestroyActor( InActor, false );
	}

	/**
	 * Spawn actor in world
	 * 
	 * @param InLocation	Location actor on spawn
	 * @param InRotation	Rotation actor on spawn
	 * @param InName		Actor name
	 */
	template< typename TClass >
	FORCEINLINE TClass* SpawnActor( const Vector& InLocation, const CRotator& InRotation = Math::rotatorZero, const CName& InName = NAME_None )
	{
		return ( TClass* )SpawnActor( TClass::StaticClass(), InLocation, InRotation, InName );
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
	FORCEINLINE bool LineTraceSingleByChannel( HitResult& OutHitResult, const Vector& InStart, const Vector& InEnd, ECollisionChannel InTraceChannel, const CollisionQueryParams& InCollisionQueryParams = CollisionQueryParams::defaultQueryParam )
	{
		return g_PhysicsScene.LineTraceSingleByChannel( OutHitResult, InStart, InEnd, InTraceChannel, InCollisionQueryParams );
	}

#if ENABLE_HITPROXY
	/**
	 * Update hit proxies id in all actors
	 */
	void UpdateHitProxiesId();
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
	/**
	 * Select actor
	 * @param InActor	Actor
	 */
	void SelectActor( AActor* InActor );

	/**
	 * Unselect actor
	 * @param InActor	Actor
	 */
	void UnselectActor( AActor* InActor );

	/**
	 * Select actors
	 * @param InActors	Array of actors
	 */
	void SelectActors( const std::vector<AActor*>& InActors );

	/**
	 * Unselect actors
	 * @param InActors	Array of actors
	 */
	void UnselectActors( const std::vector<AActor*>& InActors );

	/**
	 * Unselect all actors
	 */
	void UnselectAllActors();
#endif // WITH_EDITOR

	/**
	 * @brief Get scene manager
	 * @return Return pointer to scene manager
	 */
	FORCEINLINE class CBaseScene* GetScene() const
	{
		return scene;
	}

	/**
	 * @brief Get number of actors
	 * @return Return number of actors
	 */
	FORCEINLINE uint32 GetNumActors() const
	{
		return actors.size();
	}

	/**
	 * @brief Get actor by index
	 * 
	 * @param InIndex	Index
	 * @return Return actor
	 */
	FORCEINLINE AActor* GetActor( uint32 InIndex ) const
	{
		Assert( InIndex >= 0 && InIndex < actors.size() );
		return actors[ InIndex ];
	}

#if WITH_EDITOR
	/**
	 * @brief Get selected actors
	 * @return Return selected actors
	 */
	FORCEINLINE const std::vector<AActor*>& GetSelectedActors() const
	{
		return selectedActors;
	}

	/**
	 * @brief Get number of selected actors
	 * @return Return number of selected actors
	 */
	FORCEINLINE uint32 GetNumSelectedActors() const
	{
		return selectedActors.size();
	}
#endif // WITH_EDITOR

private:
	/**
	 * Destroy actor in world
	 * 
	 * @param InActor				Actor
	 * @param InIsIgnorePlaying		Is need ignore playing flag in actor
	 */
	void DestroyActor( AActor* InActor, bool InIsIgnorePlaying );

	bool						isBeginPlay;		/**< Is started gameplay */
	class CBaseScene*			scene;				/**< Scene manager */
	std::vector<AActor*>		actors;				/**< Array actors in world */
	std::vector<AActor*>		actorsToDestroy;	/**< Array actors which need destroy after tick */

#if WITH_EDITOR
	std::vector<AActor*>		selectedActors;		/**< Array of selected actors */
#endif // WITH_EDITOR
};

#endif // !WORLD_H