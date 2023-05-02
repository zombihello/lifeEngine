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
	 * @brief Set radius
	 * @param InRadius		Radius
	 */
	FORCEINLINE void SetRadius( float InRadius )
	{
		radius = InRadius;
	}

	/**
	 * @brief Set height
	 * @param InHeight		Height
	 */
	FORCEINLINE void SetHeight( float InHeight )
	{
		height = InHeight;
	}

	/**
	 * @brief Set cutoff
	 * @param InCutoff		Cutoff
	 */
	FORCEINLINE void SetCutoff( float InCutoff )
	{
		cutoff = InCutoff;
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

	/**
	 * @brief Get height
	 * @return Return height
	 */
	FORCEINLINE float GetHeight() const
	{
		return height;
	}

	/**
	 * @brief Get cutoff
	 * @return Return cutoff
	 */
	FORCEINLINE float GetCutoff() const
	{
		return cutoff;
	}

private:
	float		radius;		/**< Radius */
	float		height;		/**< Height */
	float		cutoff;		/**< Cutoff */
};

#endif // !SPOTLIGHTCOMPONENT_H