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

 /**
  * @ingroup Engine
  * Base class of characters
  */
class ACharacter : public AActor
{
	DECLARE_CLASS( ACharacter, AActor )

public:
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

private:
	TRefCountPtr< ABaseController >			controller;		/**< Controller of this character */
};

#endif // !CHARACTER_H