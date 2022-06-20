/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AAUDIO_H
#define AAUDIO_H

#include "Actors/Actor.h"
#include "Components/AudioComponent.h"

 /**
  * @ingroup Engine
  * Actor of audio
  */
class AAudio : public AActor
{
	DECLARE_CLASS( AAudio, AActor )

public:
	/**
	 * Constructor
	 */
	AAudio();

#if WITH_EDITOR
	/**
	 * @brief Initialize actor properties
	 * This method called only when actor spawned on cooking of map. Available only when WITH_EDITOR is 1
	 *
	 * @param InActorVars Array of actor properties to init
	 * @param InCooker Pointer to cooker for cook any resources if need
	 * @return Return if properties inited succeed and all resources cooked is succeed, else return false
	 */
	virtual bool InitProperties( const std::vector< CActorVar >& InActorVars, class CCookPackagesCommandlet* InCooker ) override;

	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from appBaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get audio component
	 * @return Return audio component
	 */
	FORCEINLINE TRefCountPtr< CAudioComponent > GetAudioComponent() const
	{
		return audioComponent;
	}

private:
	TRefCountPtr< CAudioComponent >			audioComponent;		/**< Audio component */
};

#endif // !AAUDIO_H