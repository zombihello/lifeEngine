/**
 * @file
 * @addtogroup HeliumGame HeliumGame
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef HEPLAYERCHARACTER_H
#define HEPLAYERCHARACTER_H

#include "Actors/Character.h"
#include "Components/CameraComponent.h"
#include "Components/SpriteComponent.h"

 /**
  * @ingroup HeliumGame
  * Class of player character
  */
class AHEPlayerCharacter : public ACharacter
{
	DECLARE_CLASS( AHEPlayerCharacter, ACharacter )

public:
	/**
	 * @brief Constructor
	 */
	AHEPlayerCharacter();

	/**
	 * @brief Destructor
	 */
	~AHEPlayerCharacter();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

private:
	TRefCountPtr< LCameraComponent >			cameraComponent;		/**< Camera component */
	TRefCountPtr< LSpriteComponent >			spriteComponent;		/**< Sprite component */
};

#endif // !HEPLAYERCHARACTER_H