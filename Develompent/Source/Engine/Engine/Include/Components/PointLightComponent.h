/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef POINTLIGHTCOMPONENT_H
#define POINTLIGHTCOMPONENT_H

#include "Components/LightComponent.h"

 /**
  * @ingroup Engine
  * Point light component
  */
class CPointLightComponent : public CLightComponent
{
	DECLARE_CLASS( CPointLightComponent, CLightComponent )

public:
	/**
	 * @brief Constructor
	 */
	CPointLightComponent();

	/**
	 * @brief Set radius
	 * @param InRadius		Radius
	 */
	FORCEINLINE void SetRadius( float InRadius )
	{
		radius = InRadius;
	}

	/**
	 * @brief Get light type
	 * Need override the method by child for setting light type
	 *
	 * @return Return light type
	 */
	virtual ELightType GetLightType() const override;

	/**
	 * @brief Get radius
	 * @return Return radius
	 */
	FORCEINLINE float GetRadius() const
	{
		return radius;
	}

private:
	float		radius;		/**< Radius */
};

#endif // !POINTLIGHTCOMPONENT_H