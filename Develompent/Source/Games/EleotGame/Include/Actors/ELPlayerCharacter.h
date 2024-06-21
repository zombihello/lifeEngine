/**
 * @file
 * @addtogroup EleotGame EleotGame
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ELPLAYERCHARACTER_H
#define ELPLAYERCHARACTER_H

#include "Actors/Character.h"
#include "Components/CameraComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/AudioComponent.h"

 /**
  * @ingroup EleotGame
  * Class of player character
  */
class AELPlayerCharacter : public ACharacter
{
	DECLARE_CLASS_INTRINSIC( AELPlayerCharacter, ACharacter, 0, 0, TEXT( "EleotGame" ) )

public:
	/**
	 * @brief Constructor
	 */
	AELPlayerCharacter();

	/**
	 * @brief Called before destroying the object
	 * This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process
	 */
	virtual void BeginDestroy() override;

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

private:
	CCameraComponent*		cameraComponent;				/**< Camera component */
};

#endif // !ELPLAYERCHARACTER_H