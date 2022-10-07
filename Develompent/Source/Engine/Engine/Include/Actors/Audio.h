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
#include "Components/SpriteComponent.h"

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

	/**
	 * Destructor
	 */
	virtual ~AAudio();

	/**
	 * @brief Set audio bank
	 * @param InAudioBank		Audio bank
	 */
	FORCEINLINE void SetAudioBank( const TAssetHandle<CAudioBank>& InAudioBank )
	{
		audioComponent->SetAudioBank( InAudioBank );
	}

#if WITH_EDITOR
	/**
	 * @brief Spawn asset actor
	 *
	 * @param InAsset       Asset
	 * @param InLocation    Location actor on spawn
	 * @param InRotation    Rotation actor on spawn
	 * @return Return spawned actor. If failed returning NULL
	 */
	static ActorRef_t SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation );

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

#if WITH_EDITOR
	TRefCountPtr<CSpriteComponent>			gizmoComponent;		/**< Gizmo component */
#endif // WITH_EDITOR
};

#endif // !AAUDIO_H