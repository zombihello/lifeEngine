/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENECOMPONENT_H
#define SCENECOMPONENT_H

#include "Components/ActorComponent.h"

 /**
  * @ingroup Engine
  * A SceneComponent has a transform and supports attachment, but has no rendering or collision capabilities.
  */
class LSceneComponent : public LActorComponent
{
	DECLARE_CLASS( LSceneComponent, LActorComponent )

public:
};

#endif // !SCENECOMPONENT_H