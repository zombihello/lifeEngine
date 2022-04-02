/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOCOMPONENT_H
#define AUDIOCOMPONENT_H

#include "System/AudioBuffer.h"
#include "System/AudioSource.h"
#include "Components/SceneComponent.h"

/**
 * @ingroup Engine
 * Component of work audio in engine
 */
class LAudioComponent : public LSceneComponent
{
	DECLARE_CLASS( LAudioComponent, LSceneComponent )

public:
	/**
	 * @brief Constructor
	 */
	LAudioComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

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

	/**
	 * @brief Play sound
	 */
	FORCEINLINE void Play()
	{
		source.Play();
	}

	/**
	 * @brief Pause sound
	 */
	FORCEINLINE void Pause()
	{
		source.Pause();
	}

	/**
	 * @brief Stop sound
	 */
	FORCEINLINE void Stop()
	{
		source.Stop();
	}

	/**
	 * @brief Set loop
	 * @param InIsLoop Is need loop sound
	 */
	FORCEINLINE void SetLoop( bool InIsLoop )
	{
		source.SetLoop( InIsLoop );
		bIsLoop = InIsLoop;
	}

	/**
	 * @brief Set UI sound
	 * @param InIsUISound Is this UI sound
	 */
	FORCEINLINE void SetUISound( bool InIsUISound )
	{
		source.SetRelativeToListener( InIsUISound );
		bIsUISound = InIsUISound;
	}

	/**
	 * @brief Set volume
	 * @param InVolume Volume
	 */
	FORCEINLINE void SetVolume( float InVolume )
	{
		source.SetVolume( InVolume );
		volume = InVolume;
	}

	/**
	 * @brief Set pitch
	 * @param InPitch Pitch
	 */
	FORCEINLINE void SetPitch( float InPitch )
	{
		source.SetPitch( InPitch );
		pitch = InPitch;
	}

	/**
	 * @brief Set min distance
	 * @param InMinDistance Min distance
	 */
	FORCEINLINE void SetMinDistance( float InMinDistance )
	{
		source.SetMinDistance( InMinDistance );
		minDistance = InMinDistance;
	}

	/**
	 * @brief Set attenuation
	 * @param InAttenuation Attenuation
	 */
	FORCEINLINE void SetAttenuation( float InAttenuation )
	{
		source.SetAttenuation( InAttenuation );
		attenuation = InAttenuation;
	}

	/**
	 * @brief Set audio buffer
	 * @param InAudioBuffer Audio buffer
	 */
	FORCEINLINE void SetAudioBuffer( FAudioBuffer* InAudioBuffer )
	{
		buffer = InAudioBuffer;
		source.SetAudioBuffer( buffer );
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
	 * @brief Get audio buffer
	 * @return Return audio buffer. If not setted return nullptr
	 */
	FORCEINLINE FAudioBufferRef GetAudioBuffer() const
	{
		return buffer;
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
		return source.GetStatus();
	}

private:
	bool					bIsLoop;					/**< Is looped */
	bool					bIsUISound;					/**< Is UI sound */
	bool					bIsAutoPlay;				/**< Is need auto play on begin play */
	float					volume;						/**< Volume of sound */
	float					pitch;						/**< Pitch */
	float					minDistance;				/**< Min distance */
	float					attenuation;				/**< Attenuation */
	FAudioBufferRef			buffer;						/**< Audio buffer */
	FAudioSource			source;						/**< Audio source */
	FVector					oldSourceLocation;			/**< Old source location */
};

#endif // !AUDIOCOMPONENT_H