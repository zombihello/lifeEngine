/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "Components/SceneComponent.h"

 /**
  * @ingroup Engine
  * Represents a camera viewpoint and settings, such as projection type, field of view, and post-process overrides
  */
class LCameraComponent : public LSceneComponent
{
	DECLARE_CLASS( LCameraComponent, LSceneComponent )

public:
};

#endif // !CAMERACOMPONENT_H