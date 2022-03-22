/**
 * @file
 * @addtogroup HeliumGame HeliumGame
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef HEPLAYERCONTROLLER_H
#define HEPLAYERCONTROLLER_H

#include "Actors/PlayerController.h"

 /**
  * @ingroup HeliumGame
  * Actor of HeliumGame player controller
  */
class AHEPlayerController : public APlayerController
{
	DECLARE_CLASS( AHEPlayerController, APlayerController )

public:
	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;
};

#endif // !HEPLAYERCONTROLLER_H
