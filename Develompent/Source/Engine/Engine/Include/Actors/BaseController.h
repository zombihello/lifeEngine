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
	DECLARE_CLASS( ABaseController, AActor )

public:
	/**
	 * @brief Constructor
	 */
	ABaseController();

	/**
	 * @brief Destructor
	 */
	virtual ~ABaseController();

	/**
	 * @brief Set controlled character
	 * @param InCharacter Character
	 */
	void SetCharacter( class ACharacter* InCharacter );

	/**
	 * @brief Get controlled character
	 * @return Return controlled character. If not exist return nullptr
	 */
	TRefCountPtr<class ACharacter> GetCharacter() const;

protected:
	TRefCountPtr<class ACharacter>		character;			/**< Character controlled by this controller */
};

#endif // !BASECONTROLLER_H