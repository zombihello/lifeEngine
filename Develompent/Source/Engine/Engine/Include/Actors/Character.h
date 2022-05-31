/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CHARACTER_H
#define CHARACTER_H

#include "Actors/Actor.h"
#include "Actors/BaseController.h"

#include "Components/CharacterMovementComponent.h"

 /**
  * @ingroup Engine
  * Base class of characters
  */
class ACharacter : public AActor
{
	DECLARE_CLASS( ACharacter, AActor )

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
	FORCEINLINE TRefCountPtr< ABaseController > GetController() const
	{
		return controller;
	}

	/**
	 * @brief Get character movement component
	 * @return Return character movement component
	 */
	FORCEINLINE TRefCountPtr< CCharacterMovementComponent > GetCharacterMovement() const
	{
		return characterMovement;
	}

private:
	TRefCountPtr< ABaseController >						controller;				/**< Controller of this character */
	TRefCountPtr< CCharacterMovementComponent >			characterMovement;		/**< Character movement component */
};

#endif // !CHARACTER_H