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
	DECLARE_CLASS_INTRINSIC( AAudio, AActor, 0, 0, TEXT( "Engine" ) )

public:
	/**
	 * Constructor
	 */
	AAudio();

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
	static AActor* SpawnActorAsset( const TSharedPtr<CAsset>& InAsset, const Vector& InLocation, const Quaternion& InRotation );

	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get audio component
	 * @return Return audio component
	 */
	FORCEINLINE CAudioComponent* GetAudioComponent() const
	{
		return audioComponent;
	}

private:
	CAudioComponent*			audioComponent;		/**< Audio component */

#if WITH_EDITOR
	CSpriteComponent*			gizmoComponent;		/**< Gizmo component */
#endif // WITH_EDITOR
};

#endif // !AAUDIO_H