/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ABOXCOLLISION_H
#define ABOXCOLLISION_H

#include "Actors/Actor.h"
#include "Components/BoxComponent.h"

 /**
  * @ingroup Engine
  * Actor of sprite
  */
class ABoxCollision : public AActor
{
	DECLARE_CLASS_INTRINSIC( ABoxCollision, AActor, 0, 0, TEXT( "Engine" ) )

public:
    /**
     * Constructor
     */
    ABoxCollision();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get box component
	 * @return Return box component
	 */
	FORCEINLINE CBoxComponent* GetBoxComponent() const
	{
		return boxComponent;
	}

private:
	CBoxComponent*			boxComponent;		/**< Box component */
};

#endif // !ABOXCOLLISION_H
