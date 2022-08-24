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
	DECLARE_CLASS( AELPlayerCharacter, ACharacter )

public:
	/**
	 * @brief Constructor
	 */
	AELPlayerCharacter();

	/**
	 * @brief Destructor
	 */
	~AELPlayerCharacter();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

private:
	TRefCountPtr<CCameraComponent>		cameraComponent;				/**< Camera component */
};

#endif // !ELPLAYERCHARACTER_H