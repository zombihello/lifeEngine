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
};

#endif // !POINTLIGHTCOMPONENT_H