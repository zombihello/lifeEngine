/**
 * @file
 * @addtogroup EleotGame EleotGame
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ELPLAYERCONTROLLER_H
#define ELPLAYERCONTROLLER_H

#include "Engine/Actors/PlayerController.h"

 /**
  * @ingroup EleotGame
  * Actor of EleotGame player controller
  */
class AELPlayerController : public APlayerController
{
	DECLARE_CLASS( AELPlayerController, APlayerController, 0, 0 )

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
