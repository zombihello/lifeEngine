/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DIRECTIONALLIGHTCOMPONENT_H
#define DIRECTIONALLIGHTCOMPONENT_H

#include "Components/LightComponent.h"

 /**
  * @ingroup Engine
  * Directional light component
  */
class CDirectionalLightComponent : public CLightComponent
{
	DECLARE_CLASS( CDirectionalLightComponent, CLightComponent )

public:
	/**
	 * @brief Constructor
	 */
	CDirectionalLightComponent();

	/**
	 * @brief Get light type
	 * Need override the method by child for setting light type
	 *
	 * @return Return light type
	 */
	virtual ELightType GetLightType() const override;
};

#endif // !DIRECTIONALLIGHTCOMPONENT_H