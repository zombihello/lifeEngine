/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "Math/Math.h"
#include "Misc/AudioGlobals.h"
#include "System/AudioDevice.h"
#include "System/AudioBank.h"
#include "System/AudioBuffer.h"

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
	virtual ~FAudioSource();

	/**
	 * Play
	 */
	virtual void Play();

	/**
	 * Pause
	 */
	virtual void Pause();

	/**
	 * Stop
	 */
	virtual void Stop();

	/**
	 * Set loop
	 * @param InIsLoop Is need loop sound
	 */
	virtual void SetLoop( bool InIsLoop );

	/**
	 * Set relative to listener
	 * @param InIsRelativeToListener Is this sound relative to listener
	 */
	virtual void SetRelativeToListener( bool InIsRelativeToListener );

	/**
	 * Set volume
	 * @param InVolume Volume
	 */
	virtual void SetVolume( float InVolume );

	/**
	 * Set pitch
	 * @param InPitch Pitch
	 */
	virtual void SetPitch( float InPitch );

	/**
	 * Set min distance
	 * @param InMinDistance Min distance
	 */
	virtual void SetMinDistance( float InMinDistance );

	/**
	 * Set attenuation
	 * @param InAttenuation Attenuation
	 */
	virtual void SetAttenuation( float InAttenuation );

	/**
	 * Set audio bank
	 * @param InAudioBank Audio bank
	 */
	virtual void SetAudioBank( FAudioBank* InAudioBank );

	/**
	 * Set location
	 * @param InLocation Location
	 */
	virtual void SetLocation( const FVector& InLocation );

	/**
	 * Is looped
	 * @return Return true if sound is looped, else return false
	 */
	virtual bool IsLooped() const;

	/**
	 * Is this sound relative to listener
	 * @return Return true if sound relative to listener, else return false
	 */
	virtual bool IsRelativeToListener() const;

	/**
	 * Get volume
	 * @return Return volume of sound
	 */
	virtual float GetVolume() const;

	/**
	 * Get pitch
	 * @return Return pitch
	 */
	virtual float GetPitch() const;

	/**
	 * Get min distance
	 * @return Return min distance
	 */
	virtual float GetMinDistance() const;

	/**
	 * Get attenuation
	 * @return Return attenuation
	 */
	virtual float GetAttenuation() const;

	/**
	 * Get audio source status
	 * @return Return audio source status
	 */
	virtual EAudioSourceStatus GetStatus() const;

	/**
	 * Get audio bank
	 * @return Return audio bank. If not setted return nullptr
	 */
	virtual FAudioBankRef GetAudioBank() const;

	/**
	 * Get location
	 * @return Return location of source audio
	 */
	virtual FVector GetLocation() const;

	/**
	 * Get OpenAL handle to source
	 * @return Return OpenAL handle to source
	 */
	FORCEINLINE uint32 GetALHandle() const
	{
		return alHandle;
	}

protected:
	FAudioBankRef		audioBank;		/**< Audio bank */
	float				volume;			/**< Volume */

private:
	uint32				alHandle;		/**< OpenAL of sound source */
};

#endif // !AUDIOSOURCE_H