/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H

#include "Math/Box.h"
#include "System/PhysicsBodySetup.h"
#include "System/PhysicsBodyInstance.h"
#include "Components/SceneComponent.h"

/**
 * @ingroup Engine
 * Typedef of pointer to primitive component
 */
typedef TRefCountPtr< class LPrimitiveComponent >		LPrimitiveComponentRef;

/**
 * @ingroup Engine
 * PrimitiveComponents are SceneComponents that contain or generate some sort of geometry, generally to be rendered or used as collision data.
 */
class LPrimitiveComponent : public LSceneComponent
{
	DECLARE_CLASS( LPrimitiveComponent, LSceneComponent )

public:
	/**
	 * @brief Constructor
	 */
	LPrimitiveComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * @brief Adds a draw policy link in SDGs
	 * 
	 * @param InScene Scene
	 */
	virtual void LinkDrawList( class FScene* InScene ) PURE_VIRTUAL( LPrimitiveComponent::LinkDrawList, );

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList() PURE_VIRTUAL( LPrimitiveComponent::UnlinkDrawList, );

	/**
	 * @brief Adds mesh batches for draw in scene
	 * 
     * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class FSceneView& InSceneView ) PURE_VIRTUAL( LPrimitiveComponent::AddToDrawList, );

	/**
	 * @brief Init physics component
	 */
	void InitPrimitivePhysics();

	/**
	 * @brief Move this component to match the physics rigid body pose. Note, a warning will be generated if you call this function on a component that is attached to something
	 */
	void SyncComponentToPhysics();

	/**
	 * @brief Terminate physics component
	 */
	void TermPrimitivePhysics();

	/**
	 * @brief Set visibility
	 * 
	 * @param InNewVisibility New visibility
	 */
	virtual void SetVisibility( bool InNewVisibility );

	/**
	 * @brief Set body setup
	 * @param InBodySetup Body setup
	 */
	FORCEINLINE void SetBodySetup( FPhysicsBodySetup* InBodySetup )
	{
		bDirtyBodyInstance = InBodySetup != bodySetup ? true : bDirtyBodyInstance;
		bodySetup = InBodySetup;
	}

	/**
	 * @brief Enable collision
	 * @param InIsCollision Is need enable collision
	 */
	FORCEINLINE void SetCollision( bool InIsCollision )
	{
		bDirtyBodyInstance = InIsCollision != bCollision ? true : bDirtyBodyInstance;
		bCollision = InIsCollision;
	}

	/**
	 * @brief Set body lock flags
	 * @param InBodyLockFlags Body lock flags (see EBodyLockFlag)
	 */
	FORCEINLINE void SetBodyLockFlags( uint32 InBodyLockFlags )
	{
		bodyLockFlags = InBodyLockFlags;
		if ( FPhysicsBodyInstance* bodyInstance = GetBodyInstance() )
		{
			bodyInstance->SetLockFlags( bodyLockFlags );
		}
	}

	/**
	 * @brief Set body mass
	 * @param InBodyMass Body mass
	 */
	FORCEINLINE void SetBodyMass( float InBodyMass )
	{
		bodyMass = InBodyMass;
		if ( FPhysicsBodyInstance* bodyInstance = GetBodyInstance() )
		{
			bodyInstance->SetMass( bodyMass );
		}
	}

	/**
	 * @brief Get visibility
	 * @return Return visibility of primitive
	 */
	FORCEINLINE bool IsVisibility() const
	{
		return bVisibility;
	}

	/**
	 * @brief Is enabled collision
	 * @return Return true if enabled collision, else return false
	 */
	FORCEINLINE bool IsCollision() const
	{
		return bCollision;
	}

	/**
	 * @brief Get body lock flags
	 * @return Return body lock flags
	 */
	FORCEINLINE uint32 GetBodyLockFlags() const
	{
		return bodyLockFlags;
	}

	/**
	 * @brief Get bound box
	 * @return Return bound box
	 */
	FORCEINLINE const FBox& GetBoundBox() const
	{
		return boundbox;
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
	 * @brief Get body instance
	 * @return Return pointer to body instance. If current primitive not is collision component in owner, return owner instance. If collision component is not valid return nullptr
	 */
	const FPhysicsBodyInstance* GetBodyInstance() const;

	/**
	 * @brief Get body instance
	 * @return Return pointer to body instance. If current primitive not is collision component in owner, return owner instance. If collision component is not valid return nullptr
	 */
	FPhysicsBodyInstance* GetBodyInstance();

	/**
	 * @brief Get body mass
	 * @return Return body mass
	 */
	FORCEINLINE float GetBodyMass() const
	{
		return bodyMass;
	}

protected:
	FBox						boundbox;		/**< Bound box */
	FPhysicsBodySetupRef		bodySetup;		/**< Physics body setup */
	
private:
	bool						bVisibility;		/**< Is primitive visibility */
	bool						bCollision;			/**< Is enabled collision in primitive */
	bool						bDirtyBodyInstance;	/**< Is need reinit body instance */
	uint32						bodyLockFlags;		/**< Body lock flags (see EBodyLockFlag) */
	float						bodyMass;			/**< Body mass */
	FPhysicsBodyInstance		bodyInstance;		/**< Physics body instance */
};

#endif // !PRIMITIVECOMPONENT_H
