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

#ifndef ELPLAYERCHARACTER_H
#define ELPLAYERCHARACTER_H

#include "Engine/Actors/Character.h"
#include "Engine/Components/CameraComponent.h"
#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/AudioComponent.h"

 /**
  * @ingroup EleotGame
  * Class of player character
  */
class AELPlayerCharacter : public ACharacter
{
	DECLARE_CLASS( AELPlayerCharacter, ACharacter, 0, 0 )

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