/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include "Actors/Actor.h"
#include "Misc/RefCountPtr.h"

 /**
  * @ingroup Engine
  * Base class of all controllers
  */
class ABaseController : public AActor
{
	DECLARE_CLASS( ABaseController, AActor, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * @brief Constructor
	 */
	ABaseController();

	/**
	 * @brief Set controlled character
	 * @param InCharacter Character
	 */
	void SetCharacter( class ACharacter* InCharacter );

	/**
	 * @brief Get controlled character
	 * @return Return controlled character. If not exist return nullptr
	 */
	class ACharacter* GetCharacter() const;

protected:
	class ACharacter*		character;			/**< Character controlled by this controller */
};

#endif // !BASECONTROLLER_H