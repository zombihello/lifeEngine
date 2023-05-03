/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ADIRECTIONALLIGHT_H
#define ADIRECTIONALLIGHT_H

#include "Actors/Light.h"
#include "Components/SpriteComponent.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/ArrowComponent.h"

 /**
  * @ingroup Engine
  * Actor of directional light
  */
class ADirectionalLight : public ALight
{
	DECLARE_CLASS( ADirectionalLight, ALight )

public:
	/**
	 * Constructor
	 */
	ADirectionalLight();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from appBaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get directional light component
	 * @return Return directional light component
	 */
	FORCEINLINE TRefCountPtr<CDirectionalLightComponent> GetDirectionalLightComponent() const
	{
		return directionalLightComponent;
	}

private:
	TRefCountPtr<CDirectionalLightComponent>			directionalLightComponent;		/**< Directional light component */

#if WITH_EDITOR
	TRefCountPtr<CSpriteComponent>						gizmoComponent;					/**< Gizmo component */
	TRefCountPtr<CArrowComponent>						arrowComponent;					/**< Arrow component */
#endif // WITH_EDITOR
};

#endif // !ADIRECTIONALLIGHT_H