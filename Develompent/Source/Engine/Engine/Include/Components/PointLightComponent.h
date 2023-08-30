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
	DECLARE_CLASS( CPointLightComponent, CLightComponent, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CPointLightComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

#if WITH_EDITOR
	/**
	 * @brief Function called every frame for drawing debug geometry. Override this function to implement custom logic of drawing debug geometry
	 */
	virtual void DrawDebugComponent() override;
#endif // WITH_EDITOR

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