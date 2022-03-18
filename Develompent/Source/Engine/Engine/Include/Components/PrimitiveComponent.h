/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PRIMITIVECOMPONENT_H
#define PRIMITIVECOMPONENT_H

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
	 * @brief Add primitive to draw list
	 * 
	 * @param InScene Scene
     * @param InSceneView Current view of scene
	 */
    virtual void AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView );

	/**
	 * @brief Set visibility
	 * 
	 * @param InNewVisibility New visibility
	 */
	virtual void SetVisibility( bool InNewVisibility );

	/**
	 * @brief Get visibility
	 * @return Return visibility of primitive
	 */
	FORCEINLINE bool IsVisibility() const
	{
		return bVisibility;
	}

private:
	bool		bVisibility;		/**< Is primitive visibility */
};

#endif // !PRIMITIVECOMPONENT_H
