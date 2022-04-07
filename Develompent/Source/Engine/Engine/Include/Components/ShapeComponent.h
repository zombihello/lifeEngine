/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHAPECOMPONENT_H
#define SHAPECOMPONENT_H

#include "Math/Transform.h"
#include "Components/PrimitiveComponent.h"

 /**
  * @ingroup Engine
  * ShapeComponent is a PrimitiveComponent that is represented by a simple geometrical shape (sphere, capsule, box, etc).
  */
class LShapeComponent : public LPrimitiveComponent
{
	DECLARE_CLASS( LShapeComponent, LPrimitiveComponent )

public:
	/**
	 * @brief Constructor
	 */
	LShapeComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~LShapeComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() PURE_VIRTUAL( LShapeComponent::UpdateBodySetup, );
};

#endif // !SHAPECOMPONENT_H