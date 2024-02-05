/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef CHARACTER_H
#define CHARACTER_H

#include "Engine/Actors/Actor.h"
#include "Engine/Actors/BaseController.h"

#include "Engine/Components/CharacterMovementComponent.h"

 /**
  * @ingroup Engine
  * Base class of characters
  */
class ACharacter : public AActor
{
	DECLARE_CLASS( ACharacter, AActor, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	ACharacter();
	
	/**
	 * @brief Destructor
	 */
	virtual ~ACharacter();

	/**
	 * @brief Walk
	 *
	 * @param InWorldDirection Direction of move in world space
	 * @param InScale Scale of move
	 */
	virtual void Walk( const Vector& InWorldDirection, float InScale );

	/**
	 * @brief Jump
	 */
	virtual void Jump();

	/**
	 * @brief Stop jump
	 */
	virtual void StopJump();

	/**
	 * @brief Landed
	 */
	virtual void Landed();

	/**
	 * @brief Set controller of character
	 * @param InController Controller
	 */
	FORCEINLINE void SetController( ABaseController* InController )
	{
		controller = InController;
	}

	/**
	 * @brief Get controller of character
	 * @return Return controller of character. If not exist return nullptr
	 */
	FORCEINLINE TRefCountPtr<ABaseController> GetController() const
	{
		return controller;
	}

	/**
	 * @brief Get character movement component
	 * @return Return character movement component
	 */
	FORCEINLINE TRefCountPtr<CCharacterMovementComponent> GetCharacterMovement() const
	{
		return characterMovement;
	}

private:
	TRefCountPtr<ABaseController>						controller;				/**< Controller of this character */
	TRefCountPtr<CCharacterMovementComponent>			characterMovement;		/**< Character movement component */
};

#endif // !CHARACTER_H