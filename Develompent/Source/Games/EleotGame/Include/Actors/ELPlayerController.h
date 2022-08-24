/**
 * @file
 * @addtogroup EleotGame EleotGame
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ELPLAYERCONTROLLER_H
#define ELPLAYERCONTROLLER_H

#include "Actors/PlayerController.h"

 /**
  * @ingroup EleotGame
  * Actor of EleotGame player controller
  */
class AELPlayerController : public APlayerController
{
	DECLARE_CLASS( AELPlayerController, APlayerController )

protected:
	/**
	 * @brief Setup input player
	 */
	virtual void SetupInputComponent() override;

private:
	/**
	 * @brief Exit from game
	 */
	void ExitFromGame();

	/**
	 * @brief Move right
	 * @param InValue Scale of axis
	 */
	void MoveRight( float InValue );

	/**
	 * @brief Jump
	 */
	void Jump();

	/**
	 * @brief Stop jump
	 */
	void StopJump();
};

#endif // !ELPLAYERCONTROLLER_H
