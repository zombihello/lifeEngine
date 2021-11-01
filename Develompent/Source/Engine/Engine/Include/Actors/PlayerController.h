/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Actors/Actor.h"

/**
 * @ingroup Engine
 * Actor of base implementation player controller
 */
class APlayerController : public AActor
{
	DECLARE_CLASS( APlayerController, AActor )

public:
	/**
	 * Constructor
	 */
	APlayerController();

	/**
	 * Destructor
	 */
	virtual ~APlayerController();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime ) override;

private:
	TRefCountPtr< class LCameraComponent >			cameraComponent;		/**< Player camera */
};


#endif // !PLAYERCONTROLLER_H