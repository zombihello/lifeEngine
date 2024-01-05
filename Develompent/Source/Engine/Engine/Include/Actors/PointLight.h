/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef APOINTLIGHT_H
#define APOINTLIGHT_H

#include "Actors/Light.h"
#include "Components/SpriteComponent.h"
#include "Components/PointLightComponent.h"

 /**
  * @ingroup Engine
  * Actor of point light
  */
class APointLight : public ALight
{
	DECLARE_CLASS( APointLight, ALight, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	APointLight();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get point light component
	 * @return Return point light component
	 */
	FORCEINLINE CPointLightComponent* GetPointLightComponent() const
	{
		return pointLightComponent;
	}

private:
	CPointLightComponent*		pointLightComponent;		/**< Point light component */

#if WITH_EDITOR
	CSpriteComponent*			gizmoComponent;				/**< Gizmo component */
#endif // WITH_EDITOR
};

#endif // !APOINTLIGHT_H