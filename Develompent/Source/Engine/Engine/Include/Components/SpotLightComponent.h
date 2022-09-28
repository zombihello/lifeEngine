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
};

#endif // !SPOTLIGHTCOMPONENT_H