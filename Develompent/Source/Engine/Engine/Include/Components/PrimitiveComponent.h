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
  * PrimitiveComponents are SceneComponents that contain or generate some sort of geometry, generally to be rendered or used as collision data.
  */
class LPrimitiveComponent : public LSceneComponent
{
	DECLARE_CLASS( LPrimitiveComponent, LSceneComponent )

public:
};

#endif // !PRIMITIVECOMPONENT_H