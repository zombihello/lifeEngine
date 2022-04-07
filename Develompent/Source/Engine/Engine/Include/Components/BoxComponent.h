/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOXCOMPONENT_H
#define BOXCOMPONENT_H

#include "Components/ShapeComponent.h"

 /**
  * @ingroup Engine
  * Box collision shape
  */
class LBoxComponent : public LShapeComponent
{
	DECLARE_CLASS( LBoxComponent, LShapeComponent )

public:
	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() override;
};

#endif // !SHAPECOMPONENT_H