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
	DECLARE_CLASS_INTRINSIC( AELPlayerController, APlayerController, 0, 0, TEXT( "EleotGame" ) )

public:
	/**
	 * @brief Constructor
	 */
	AELPlayerController();

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
	 * @brief Move forward
	 * @param InValue Scale of axis
	 */
	void MoveForward( float InValue );

	/**
	 * @brief Move right
	 * @param InValue Scale of axis
	 */
	void MoveRight( float InValue );

	/**
	 * @brief Turn
	 * @param InValue Scale of axis
	 */
	void Turn( float InValue );

	/**
	 * @brief Look up
	 * @param InValue Scale of axis
	 */
	void LookUp( float InValue );

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
