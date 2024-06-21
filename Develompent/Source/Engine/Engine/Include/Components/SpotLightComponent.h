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
	DECLARE_CLASS_INTRINSIC( CSpotLightComponent, CLightComponent, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * @brief Constructor
	 */
	CSpotLightComponent();

	/**
	 * @brief Do any object-specific cleanup required immediately after loading an object
	 * @note This is not called for newly-created objects, and by default will always execute on the game thread
	 */
	virtual void PostLoad() override;

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;

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
		bNeedUpdateCutoff = true;
	}

	/**
	 * @brief Set height
	 * @param InHeight		Height
	 */
	FORCEINLINE void SetHeight( float InHeight )
	{
		height = InHeight;
		bNeedUpdateCutoff = true;
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
		if ( bNeedUpdateCutoff )
		{
			cutoff = height / ( Math::Sqrt( Math::Pow( height, 2.f ) + Math::Pow( radius, 2 ) ) );
			bNeedUpdateCutoff = false;
		}
		return cutoff;
	}

private:
	mutable bool	bNeedUpdateCutoff;	/**< Is need update cutoff */
	float			radius;				/**< Radius */
	float			height;				/**< Height */
	mutable float	cutoff;				/**< Cutoff */
};

#endif // !SPOTLIGHTCOMPONENT_H