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
		bodySetup = InBodySetup;
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
	 * @return Return pointer to body instance
	 */
	FORCEINLINE const FPhysicsBodyInstance* GetBodyInstance() const
	{
		return &bodyInstance;
	}

protected:
	FBox						boundbox;		/**< Bound box */
	FPhysicsBodySetupRef		bodySetup;		/**< Physics body setup */
	FPhysicsBodyInstance		bodyInstance;	/**< Physics body instance */

private:
	bool						bVisibility;		/**< Is primitive visibility */
};

#endif // !PRIMITIVECOMPONENT_H
