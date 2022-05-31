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
#include "Actors/Character.h"
#include "Components/InputComponent.h"

/**
 * @ingroup Engine
 * Actor of base implementation player controller
 */
class APlayerController : public ABaseController
{
	DECLARE_CLASS( APlayerController, ABaseController )

public:
	/**
	 * Constructor
	 */
	APlayerController();

	/**
	 * Destructor
	 */
	virtual ~APlayerController();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Set controlled character
	 * @param InCharacter Character
	 */
	FORCEINLINE void SetCharacter( ACharacter* InCharacter )
	{
		if ( character )
		{
			character->SetController( nullptr );
		}

		character = InCharacter;
	
		if ( character )
		{
			character->SetController( this );
		}
	}

	/**
	 * @brief Get input component
	 * @return Return input component
	 */
	FORCEINLINE TRefCountPtr< CInputComponent > GetInputComponent() const
	{
		return inputComponent;
	}

	/**
	 * @brief Get controlled character
	 * @return Return controlled character. If not exist return nullptr
	 */
	FORCEINLINE TRefCountPtr< ACharacter > GetCharacter() const
	{
		return character;
	}

protected:
	/**
	 * @brief Setup input player
	 */
	virtual void SetupInputComponent();

	TRefCountPtr< ACharacter >					character;				/**< Character controlled by this controller */
	TRefCountPtr< CInputComponent >				inputComponent;			/**< Input component */
};


#endif // !PLAYERCONTROLLER_H
