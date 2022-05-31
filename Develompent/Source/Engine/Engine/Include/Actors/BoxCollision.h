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
    DECLARE_CLASS( ABoxCollision, AActor )

public:
    /**
     * Constructor
     */
    ABoxCollision();

#if WITH_EDITOR
	/**
	 * @brief Initialize actor properties
	 * This method called only when actor spawned on cooking of map. Available only when WITH_EDITOR is 1
	 *
	 * @param InActorVars Array of actor properties to init
	 * @param InCooker Pointer to cooker for cook any resources if need
	 * @return Return if properties inited succeed and all resources cooked is succeed, else return false
	 */
	virtual bool InitProperties( const std::vector< CActorVar >& InActorVars, class CCookPackagesCommandlet* InCooker ) override;
#endif // WITH_EDITOR

	/**
	 * Get box component
	 * @return Return box component
	 */
	FORCEINLINE TRefCountPtr< CBoxComponent > GetBoxComponent() const
	{
		return boxComponent;
	}

private:
	TRefCountPtr< CBoxComponent >			boxComponent;		/**< Box component */
};

#endif // !ABOXCOLLISION_H
