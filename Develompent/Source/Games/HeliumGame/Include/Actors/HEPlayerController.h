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

#endif // !HEPLAYERCONTROLLER_H
