/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include "Actors/BaseController.h"
#include "Components/InputComponent.h"

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
