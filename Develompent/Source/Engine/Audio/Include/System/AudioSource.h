/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include <AL/al.h>
#include <AL/alc.h>

#include "Math/Math.h"
#include "Misc/AudioGlobals.h"
#include "System/AudioDevice.h"
#include "System/AudioBank.h"
#include "System/AudioBuffer.h"

 /**
  * @ingroup Audio
  * @brief Enumeration of status audio source
  */
enum EAudioSourceStatus
{
	ASS_Playing,		/**< Audio source is playing */
	ASS_Paused,			/**< Audio source on pause */
	ASS_Stoped			/**< Audio source is stoped */
};

/**
 * @ingroup Audio
 * @brief Audio source
 */
class FAudioSource
{
public:
	/**
	 * Constructor
	 */
	FAudioSource();

	/**
	 * Destructor
	 */
	~FAudioSource();

	/**
	 * Play
	 */
	FORCEINLINE void Play()
	{
		alSourcePlay( alHandle );
	}

	/**
	 * Pause
	 */
	FORCEINLINE void Pause()
	{
		alSourcePause( alHandle );
	}

	/**
	 * Stop
	 */
	FORCEINLINE void Stop()
	{
		alSourceStop( alHandle );
	}

	/**
	 * Set loop
	 * @param InIsLoop Is need loop sound
	 */
	FORCEINLINE void SetLoop( bool InIsLoop )
	{
		alSourcei( alHandle, AL_LOOPING, InIsLoop );
	}

	/**
	 * Set relative to listener
	 * @param InIsRelativeToListener Is this sound relative to listener
	 */
	FORCEINLINE void SetRelativeToListener( bool InIsRelativeToListener )
	{
		alSourcei( alHandle, AL_SOURCE_RELATIVE, InIsRelativeToListener );
	}

	/**
	 * Set volume
	 * @param InVolume Volume
	 */
	FORCEINLINE void SetVolume( float InVolume )
	{
		alSourcef( alHandle, AL_GAIN, InVolume * GAudioDevice.GetPlatformAudioHeadroom() * 0.01f );
		volume = InVolume;
	}

	/**
	 * Set pitch
	 * @param InPitch Pitch
	 */
	FORCEINLINE void SetPitch( float InPitch )
	{
		alSourcef( alHandle, AL_PITCH, InPitch );
	}

	/**
	 * Set min distance
	 * @param InMinDistance Min distance
	 */
	FORCEINLINE void SetMinDistance( float InMinDistance )
	{
		alSourcef( alHandle, AL_REFERENCE_DISTANCE, InMinDistance );
	}

	/**
	 * Set attenuation
	 * @param InAttenuation Attenuation
	 */
	FORCEINLINE void SetAttenuation( float InAttenuation )
	{
		alSourcef( alHandle, AL_ROLLOFF_FACTOR, InAttenuation );
	}

	/**
	 * Set audio bank
	 * @param InAudioBank Audio bank
	 */
	FORCEINLINE void SetAudioBank( FAudioBank* InAudioBank )
	{
		FAudioBufferRef		audioBuffer;
		audioBank = InAudioBank;
		
		if ( audioBank )
		{
			audioBuffer = audioBank->GetAudioBuffer();
		}
		alSourcei( alHandle, AL_BUFFER, audioBuffer ? audioBuffer->GetALHandle() : 0 );
	}

	/**
	 * Set location
	 * @param InLocation Location
	 */
	FORCEINLINE void SetLocation( const FVector& InLocation )
	{
		alSource3f( alHandle, AL_POSITION, InLocation.x, InLocation.y, InLocation.z );
	}

	/**
	 * Is looped
	 * @return Return true if sound is looped, else return false
	 */
	FORCEINLINE bool IsLooped() const
	{
		ALint			isLoop = 0;
		alGetSourcei( alHandle, AL_LOOPING, &isLoop );
		return isLoop != 0;
	}

	/**
	 * Is this sound relative to listener
	 * @return Return true if sound relative to listener, else return false
	 */
	FORCEINLINE bool IsRelativeToListener() const
	{
		ALint			isRelativeToListener = 0;
		alGetSourcei( alHandle, AL_SOURCE_RELATIVE, &isRelativeToListener );
		return isRelativeToListener != 0;
	}

	/**
	 * Get volume
	 * @return Return volume of sound
	 */
	FORCEINLINE float GetVolume() const
	{
		return volume;
	}

	/**
	 * Get pitch
	 * @return Return pitch
	 */
	FORCEINLINE float GetPitch() const
	{
		ALfloat			pitch = 0.f;
		alGetSourcef( alHandle, AL_PITCH, &pitch );
		return pitch;
	}

	/**
	 * Get min distance
	 * @return Return min distance
	 */
	FORCEINLINE float GetMinDistance() const
	{
		ALfloat			distance = 0.f;
		alGetSourcef( alHandle, AL_REFERENCE_DISTANCE, &distance );
		return distance;
	}

	/**
	 * Get attenuation
	 * @return Return attenuation
	 */
	FORCEINLINE float GetAttenuation() const
	{
		ALfloat			attenuation = 0.f;
		alGetSourcef( alHandle, AL_ROLLOFF_FACTOR, &attenuation );
		return attenuation;
	}

	/**
	 * Get audio source status
	 * @return Return audio source status
	 */
	FORCEINLINE EAudioSourceStatus GetStatus() const
	{
		ALint			alStatus = 0;
		alGetSourcei( alHandle, AL_SOURCE_STATE, &alStatus );

		switch ( alStatus )
		{
		case AL_PLAYING:	return ASS_Playing;
		case AL_PAUSED:		return ASS_Paused;		
		case AL_STOPPED:
		default:			return ASS_Stoped;
		}
	}

	/**
	 * Get audio bank
	 * @return Return audio bank. If not setted return nullptr
	 */
	FORCEINLINE FAudioBankRef GetAudioBank() const
	{
		return audioBank;
	}

	/**
	 * Get location
	 * @return Return location of source audio
	 */
	FORCEINLINE FVector GetLocation() const
	{
		FVector			location;
		alGetSource3f( alHandle, AL_POSITION, &location.x, &location.y, &location.z );
		return location;
	}

private:
	uint32				alHandle;		/**< OpenAL of sound source */
	FAudioBankRef		audioBank;		/**< Audio bank */
	float				volume;			/**< Volume */
};

#endif // !AUDIOSOURCE_H