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
    DECLARE_CLASS( ABoxCollision, AActor, 0, 0, TEXT( "Engine" ) )

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
