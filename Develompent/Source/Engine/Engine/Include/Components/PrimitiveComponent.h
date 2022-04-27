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
	friend class FScene;			// For add and remove primitive in scene

	/**
	 * @brief Constructor
	 */
	LPrimitiveComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~LPrimitiveComponent();

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
	 * @brief Adds mesh batches for draw in scene
	 * 
     * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class FSceneView& InSceneView );

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
	FORCEINLINE void SetVisibility( bool InNewVisibility )
	{
		bVisibility = InNewVisibility;
	}

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
	 * @return Return body instance
	 */
	FORCEINLINE const FPhysicsBodyInstance& GetBodyInstance() const
	{
		return bodyInstance;
	}

	/**
	 * @brief Get body instance
	 * @return Return body instance
	 */
	FORCEINLINE FPhysicsBodyInstance& GetBodyInstance()
	{
		return bodyInstance;
	}

	/**
	 * @brief Get scene
	 * @return Return scene. If primitive not added to scene return NULL
	 */
	FORCEINLINE class FScene* GetScene() const
	{
		return scene;
	}

protected:
	/**
	 * @brief Adds a draw policy link in SDGs
	 */
	virtual void LinkDrawList();

	/**
	 * @brief Removes a draw policy link from SDGs
	 */
	virtual void UnlinkDrawList();

	bool						bVisibility;					/**< Is primitive visibility */
	bool						bIsDirtyDrawingPolicyLink;		/**< Is dirty drawing policy link. If flag equal true - need update drawing policy link */
	FBox						boundbox;						/**< Bound box */
	FPhysicsBodySetupRef		bodySetup;						/**< Physics body setup */
	FPhysicsBodyInstance		bodyInstance;					/**< Physics body instance */	
	class FScene*				scene;							/**< The current scene where the primitive is located  */
};

#endif // !PRIMITIVECOMPONENT_H
