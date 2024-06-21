/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASPOTLIGHT_H
#define ASPOTLIGHT_H

#include "Actors/Light.h"
#include "Components/SpriteComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/ArrowComponent.h"

 /**
  * @ingroup Engine
  * Actor of spot light
  */
class ASpotLight : public ALight
{
	DECLARE_CLASS_INTRINSIC( ASpotLight, ALight, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	ASpotLight();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get spot light component
	 * @return Return spot light component
	 */
	FORCEINLINE CSpotLightComponent* GetSpotLightComponent() const
	{
		return spotLightComponent;
	}

private:
	CSpotLightComponent*	spotLightComponent;		/**< Spot light component */

#if WITH_EDITOR
	CSpriteComponent*		gizmoComponent;			/**< Gizmo component */
	CArrowComponent*		arrowComponent;			/**< Arrow component */
#endif // WITH_EDITOR
};

#endif // !ASPOTLIGHT_H