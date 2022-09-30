/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPOTLIGHTCOMPONENT_H
#define SPOTLIGHTCOMPONENT_H

#include "Components/LightComponent.h"

 /**
  * @ingroup Engine
  * Spot light component
  */
class CSpotLightComponent : public CLightComponent
{
	DECLARE_CLASS( CSpotLightComponent, CLightComponent )

public:
	/**
	 * @brief Constructor
	 */
	CSpotLightComponent();

	/**
	 * @brief Get light type
	 * Need override the method by child for setting light type
	 *
	 * @return Return light type
	 */
	virtual ELightType GetLightType() const override;
};

#endif // !SPOTLIGHTCOMPONENT_H