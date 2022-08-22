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
#include "Misc/EngineTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Archive.h"
#include "Actors/Actor.h"
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
	ActorRef_t SpawnActor( class CClass* InClass, const Vector& InLocation, const Quaternion& InRotation = SMath::quaternionZero );

	/**
	 * Destroy actor in world
	 * @param InActor	Actor
	 */
	FORCEINLINE void DestroyActor( ActorRef_t InActor )
	{
		DestroyActor( InActor, false );
	}

	/**
	 * Spawn actor in world
	 * 
	 * @param[in] InLocation Location actor on spawn
	 * @param[in] InRotation Rotation actor on spawn
	 */
	template< typename TClass >
	FORCEINLINE TRefCountPtr< TClass > SpawnActor( const Vector& InLocation, const Quaternion& InRotation = SMath::quaternionZero )
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
	void SelectActor( ActorRef_t InActor );

	/**
	 * Unselect actor
	 * @param InActor	Actor
	 */
	void UnselectActor( ActorRef_t InActor );

	/**
	 * Select actors
	 * @param InActors	Array of actors
	 */
	void SelectActors( const std::vector<ActorRef_t>& InActors );

	/**
	 * Unselect actors
	 * @param InActors	Array of actors
	 */
	void UnselectActors( const std::vector<ActorRef_t>& InActors );

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
	FORCEINLINE ActorRef_t GetActor( uint32 InIndex ) const
	{
		check( InIndex >= 0 && InIndex < actors.size() );
		return actors[ InIndex ];
	}

#if WITH_EDITOR
	/**
	 * @brief Get selected actors
	 * @return Return selected actors
	 */
	FORCEINLINE const std::vector<ActorRef_t>& GetSelectedActors() const
	{
		return selectedActors;
	}

	/**
	 * @brief Get name
	 * @return Return name of world
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get path to file
	 * @return Return path to file with world. If not serialized returning empty string
	 */
	FORCEINLINE const std::wstring& GetFilePath() const
	{
		return filePath;
	}

	/**
	 * @brief Mark dirty world
	 */
	FORCEINLINE void MarkDirty()
	{
		bDirty = true;
	}

	/**
	 * @brief Is dirty world
	 * @return Return TRUE if world is dirty, else returning FALSE
	 */
	FORCEINLINE bool IsDirty() const
	{
		return bDirty;
	}
#endif // WITH_EDITOR

private:
	/**
	 * Destroy actor in world
	 * 
	 * @param InActor				Actor
	 * @param InIsIgnorePlaying		Is need ignore playing flag in actor
	 */
	void DestroyActor( ActorRef_t InActor, bool InIsIgnorePlaying );

	bool						isBeginPlay;		/**< Is started gameplay */
	class CBaseScene*			scene;				/**< Scene manager */
	std::vector<ActorRef_t>		actors;				/**< Array actors in world */
	std::vector<ActorRef_t>		actorsToDestroy;	/**< Array actors which need destroy after tick */

#if WITH_EDITOR
	bool						bDirty;				/**< Is world dirty and need save */
	std::vector<ActorRef_t>		selectedActors;		/**< Array of selected actors */
	std::wstring				name;				/**< World name */
	std::wstring				filePath;			/**< Path to serialized world on HDD */
#endif // WITH_EDITOR
};

#endif // !WORLD_H