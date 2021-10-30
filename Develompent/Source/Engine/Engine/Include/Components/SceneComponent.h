/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include "Math/Transform.h"
#include "Components/ActorComponent.h"

 /**
  * @ingroup Engine
  * A SceneComponent has a transform and supports attachment, but has no rendering or collision capabilities.
  */
class LSceneComponent : public LActorComponent
{
	DECLARE_CLASS( LSceneComponent, LActorComponent )

public:
	/**
	 * Move component
	 * 
	 * @param[in] InLocationDelta Location delta
	 */
	FORCEINLINE void MoveComponent( const FVector& InLocationDelta )
	{
		transform.AddToTranslation( InLocationDelta );
	}

	/**
	 * Get the current transform for this component
	 * @return Return current transform for this component
	 */
	FORCEINLINE const FTransform& GetComponentTransform() const
	{
		return transform;
	}

	/**
	 * Get location of the component
	 * @return Return location of the component
	 */
	FORCEINLINE FVector GetComponentLocation() const
	{
		return transform.GetLocation();
	}

	/**
	 * Get rotation of the component
	 * @return Return rotation of the component
	 */
	FORCEINLINE FQuaternion GetComponentRotation() const
	{
		return transform.GetRotation();
	}

	/**
	 * Get scale of the component
	 * @return Return scale of the component
	 */
	FORCEINLINE FVector GetComponentScale() const
	{
		return transform.GetScale();
	}

private:
	FTransform			transform;		/**< Transform of component */
};

#endif // !SCENECOMPONENT_H