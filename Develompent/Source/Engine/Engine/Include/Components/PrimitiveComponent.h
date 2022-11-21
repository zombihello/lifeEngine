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
typedef TRefCountPtr< class CPrimitiveComponent >		PrimitiveComponentRef_t;

/**
 * @ingroup Engine
 * PrimitiveComponents are SceneComponents that contain or generate some sort of geometry, generally to be rendered or used as collision data.
 */
class CPrimitiveComponent : public CSceneComponent
{
	DECLARE_CLASS( CPrimitiveComponent, CSceneComponent )

public:
	friend class CScene;			// For add and remove primitive in scene

	/**
	 * @brief Constructor
	 */
	CPrimitiveComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~CPrimitiveComponent();

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
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Adds mesh batches for draw in scene
	 * 
     * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class CSceneView& InSceneView );

	/**
	 * @brief Called when the owning Actor is spawned
	 */
	virtual void Spawned() override;

	/**
	 * @brief Called when this component is destroyed
	 */
	virtual void Destroyed() override;

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
	FORCEINLINE void SetBodySetup( CPhysicsBodySetup* InBodySetup )
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
	FORCEINLINE const CBox& GetBoundBox() const
	{
		return boundbox;
	}

	/**
	 * @brief Get body setup
	 * @return Return body setup
	 */
	FORCEINLINE PhysicsBodySetupRef_t GetBodySetup() const
	{
		return bodySetup;
	}

	/**
	 * @brief Get body instance
	 * @return Return body instance
	 */
	FORCEINLINE const CPhysicsBodyInstance& GetBodyInstance() const
	{
		return bodyInstance;
	}

	/**
	 * @brief Get body instance
	 * @return Return body instance
	 */
	FORCEINLINE CPhysicsBodyInstance& GetBodyInstance()
	{
		return bodyInstance;
	}

	/**
	 * @brief Get scene
	 * @return Return scene. If primitive not added to scene return NULL
	 */
	FORCEINLINE class CScene* GetScene() const
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
	CBox						boundbox;						/**< Bound box */
	PhysicsBodySetupRef_t		bodySetup;						/**< Physics body setup */
	CPhysicsBodyInstance		bodyInstance;					/**< Physics body instance */	
	class CScene*				scene;							/**< The current scene where the primitive is located  */
};

#endif // !PRIMITIVECOMPONENT_H
