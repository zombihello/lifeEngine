/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOCOMPONENT_H
#define AUDIOCOMPONENT_H

#include "System/AudioBank.h"
#include "System/AudioSource.h"
#include "System/AudioStreamSource.h"
#include "Components/SceneComponent.h"

/**
 * @ingroup Engine
 * Component of work audio in engine
 */
class CAudioComponent : public CSceneComponent
{
	DECLARE_CLASS( CAudioComponent, CSceneComponent, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	CAudioComponent();

	/**
	 * @brief Destructor
	 */
	~CAudioComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay() override;

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 *
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime ) override;

#if WITH_EDITOR
	/**
	 * @brief Function called by the editor when property is changed
	 * @param InPropertyChangedEvenet    Property changed event
	 */
	virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet ) override;
#endif // WITH_EDITOR

	/**
	 * @brief Play sound
	 */
	FORCEINLINE void Play()
	{
		source->Play();
	}

	/**
	 * @brief Pause sound
	 */
	FORCEINLINE void Pause()
	{
		source->Pause();
	}

	/**
	 * @brief Stop sound
	 */
	FORCEINLINE void Stop()
	{
		source->Stop();
	}

	/**
	 * @brief Set loop
	 * @param InIsLoop Is need loop sound
	 */
	FORCEINLINE void SetLoop( bool InIsLoop )
	{
		source->SetLoop( InIsLoop );
		bIsLoop = InIsLoop;
	}

	/**
	 * @brief Set UI sound
	 * @param InIsUISound Is this UI sound
	 */
	FORCEINLINE void SetUISound( bool InIsUISound )
	{
		source->SetRelativeToListener( InIsUISound );
		bIsUISound = InIsUISound;
	}

	/**
	 * @brief Set volume
	 * @param InVolume Volume
	 */
	FORCEINLINE void SetVolume( float InVolume )
	{
		source->SetVolume( InVolume );
		volume = InVolume;
	}

	/**
	 * @brief Set pitch
	 * @param InPitch Pitch
	 */
	FORCEINLINE void SetPitch( float InPitch )
	{
		source->SetPitch( InPitch );
		pitch = InPitch;
	}

	/**
	 * @brief Set min distance
	 * @param InMinDistance Min distance
	 */
	FORCEINLINE void SetMinDistance( float InMinDistance )
	{
		source->SetMinDistance( InMinDistance );
		minDistance = InMinDistance;
	}

	/**
	 * @brief Set attenuation
	 * @param InAttenuation Attenuation
	 */
	FORCEINLINE void SetAttenuation( float InAttenuation )
	{
		source->SetAttenuation( InAttenuation );
		attenuation = InAttenuation;
	}

	/**
	 * @brief Set audio bank
	 * @param InAudioBank Audio bank
	 */
	FORCEINLINE void SetAudioBank( const TAssetHandle<CAudioBank>& InAudioBank )
	{
		bank = InAudioBank;
		source->SetAudioBank( bank );
	}

	/**
	 * @brief Set auto play
	 * @param InIsAutoPlay Is need auto play on begin play
	 */
	FORCEINLINE void SetAutoPlay( bool InIsAutoPlay )
	{
		bIsAutoPlay = InIsAutoPlay;
	}

	/**
	 * @brief Set is streamable audio
	 * @param InIsStreamable Is streamable audio
	 */
	FORCEINLINE void SetStreamable( bool InIsStreamable )
	{
		bool		oldFlag = bIsStreamable;
		bIsStreamable = InIsStreamable;
		if ( bIsStreamable != oldFlag )
		{
			UpdateAudioSourceType();
		}
	}

	/**
	 * @brief Get audio bank
	 * @return Return audio bank. If not setted return nullptr
	 */
	FORCEINLINE TAssetHandle<CAudioBank> GetAudioBank() const
	{
		return bank;
	}

	/**
	 * @brief Is looped
	 * @return Return true if sound is looped, else return false
	 */
	FORCEINLINE bool IsLooped() const
	{
		return bIsLoop;
	}

	/**
	 * @brief Is UI sound
	 * @return Return true if is UI sound, else return false
	 */
	FORCEINLINE bool IsUISound() const
	{
		return bIsUISound;
	}

	/**
	 * @brief Is auto play
	 * @return Return true if sound auto play on begin play, else return false
	 */
	FORCEINLINE bool IsAutoPlay() const
	{
		return bIsAutoPlay;
	}

	/**
	 * @brief Get volume
	 * @return Return volume of sound
	 */
	FORCEINLINE float GetVolume() const
	{
		return volume;
	}

	/**
	 * @brief Get pitch
	 * @return Return pitch
	 */
	FORCEINLINE float GetPitch() const
	{
		return pitch;
	}

	/**
	 * @brief Get min distance
	 * @return Return min distance
	 */
	FORCEINLINE float GetMinDistance() const
	{
		return minDistance;
	}

	/**
	 * @brief Get attenuation
	 * @return Return attenuation
	 */
	FORCEINLINE float GetAttenuation() const
	{
		return attenuation;
	}

	/**
	 * @brief Get audio source status
	 * @return Return audio source status
	 */
	FORCEINLINE EAudioSourceStatus GetStatus() const
	{
		return source->GetStatus();
	}

	/**
	 * @brief Is streamable audio
	 * @return Return true if audio is streamable, else return false
	 */
	FORCEINLINE bool IsStreamable() const
	{
		return bIsStreamable;
	}

private:
	/**
	 * Update audio source type
	 */
	void UpdateAudioSourceType();

	bool						bIsLoop;					/**< Is looped */
	bool						bIsUISound;					/**< Is UI sound */
	bool						bIsAutoPlay;				/**< Is need auto play on begin play */
	bool						bIsStreamable;				/**< Is streamable */
	float						volume;						/**< Volume of sound */
	float						pitch;						/**< Pitch */
	float						minDistance;				/**< Min distance */
	float						attenuation;				/**< Attenuation */
	TAssetHandle<CAudioBank>	bank;						/**< Audio bank */
	CAudioSource*				source;						/**< Audio source */
	Vector						oldSourceLocation;			/**< Old source location */
};

#endif // !AUDIOCOMPONENT_H