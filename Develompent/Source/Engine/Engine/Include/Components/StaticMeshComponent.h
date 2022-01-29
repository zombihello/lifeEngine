/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHCOMPONENT_H
#define STATICMESHCOMPONENT_H

#include "Components/PrimitiveComponent.h"
#include "Render/StaticMesh.h"

 /**
  * @ingroup Engine
  * @brief Component for work with static mesh
  */
class LStaticMeshComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LStaticMeshComponent, LPrimitiveComponent )

public:
	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime ) override;

	/**
	 * @brief Add primitive to draw list
	 *
	 * @param InScene Scene
	 */
	virtual void AddToDrawList( class FScene* InScene ) override;

	/**
	 * @brief Set static mesh
	 * @param InNewStaticMesh New static mesh
	 */
	FORCEINLINE void SetStaticMesh( class FStaticMesh* InNewStaticMesh )
	{
		staticMesh = InNewStaticMesh;
	}

	/**
	 * @brief Get static mesh
	 * @return Return pointer to static mesh. If not valid returning nullptr
	 */
	FORCEINLINE FStaticMeshRef GetStaticMesh() const
	{
		return staticMesh;
	}

private:
	FStaticMeshRef			staticMesh;			/**< Static mesh */
};

#endif // !STATICMESHCOMPONENT_H