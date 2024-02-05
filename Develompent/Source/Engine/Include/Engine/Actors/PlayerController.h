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

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Engine/Actors/BaseController.h"
#include "Engine/Components/InputComponent.h"

/**
 * @ingroup Engine
 * Actor of base implementation player controller
 */
class APlayerController : public ABaseController
{
	DECLARE_CLASS( APlayerController, ABaseController, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	APlayerController();

	/**
	 * @brief Destructor
	 */
	virtual ~APlayerController();

	/**
	 * @brief Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Function called every frame on this Actor. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void Tick( float InDeltaTime );

	/**
	 * @brief Get input component
	 * @return Return input component
	 */
	FORCEINLINE TRefCountPtr< CInputComponent > GetInputComponent() const
	{
		return inputComponent;
	}

	/**
	 * @brief Set show mouse cursor
	 * @param InShowMouseCursor		Is need show mouse cursor
	 */
	FORCEINLINE void SetShowMouseCursor( bool InShowMouseCursor )
	{
		bShowMouseCursor = InShowMouseCursor;
	}

	/**
	 * @brief Is show mouse cursor
	 * @return Return TRUE if mouse cursor need show
	 */
	FORCEINLINE bool IsShowMouseCursor() const
	{
		return bShowMouseCursor;
	}

	/**
	 * @brief Add pitch input
	 * @param InValue	Value
	 */
	FORCEINLINE void AddPitchInput( float InValue )
	{
		rotationInput.x += InValue;
	}

	/**
	 * @brief Add yaw input
	 * @param InValue	Value
	 */
	FORCEINLINE void AddYawInput( float InValue )
	{
		rotationInput.y += InValue;
	}

	/**
	 * @brief Add roll input
	 * @param InValue	Value
	 */
	FORCEINLINE void AddRollInput( float InValue )
	{
		rotationInput.z += InValue;
	}

protected:
	/**
	 * @brief Setup input player
	 */
	virtual void SetupInputComponent();

	bool										bShowMouseCursor;		/**< Show mouse cursor */
	bool										bConstrainYaw;			/**< Is need constrain yaw */
	Vector										rotationInput;			/**< Rotation input */
	TRefCountPtr<CInputComponent>				inputComponent;			/**< Input component */

private:
	/**
	 * @brief Updates the rotation of player, based on ControlRotation after RotationInput has been applied.
	 * @param InDeltaTime	The time since the last tick.
	 */
	void UpdateRotation( float InDeltaTime );

	Vector										viewRotation;			/**< View rotation in Euler angles */
};


#endif // !PLAYERCONTROLLER_H
