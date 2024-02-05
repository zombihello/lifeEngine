/**
 * @file
 * @addtogroup Audio Audio
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "Core/Math/Math.h"
#include "Audio/Misc/AudioGlobals.h"
#include "Audio/System/AudioDevice.h"
#include "Audio/System/AudioBank.h"
#include "Audio/System/AudioBuffer.h"

/**
 * @ingroup Audio
 * @brief Audio source
 */
class CAudioSource
{
public:
	/**
	 * Constructor
	 */
	CAudioSource();

	/**
	 * Destructor
	 */
	virtual ~CAudioSource();

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
	virtual void SetAudioBank( const TAssetHandle<CAudioBank>& InAudioBank );

	/**
	 * Set location
	 * @param InLocation Location
	 */
	virtual void SetLocation( const Vector& InLocation );

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
	virtual TAssetHandle<CAudioBank> GetAudioBank() const;

	/**
	 * Get location
	 * @return Return location of source audio
	 */
	virtual Vector GetLocation() const;

	/**
	 * Get OpenAL handle to source
	 * @return Return OpenAL handle to source
	 */
	FORCEINLINE uint32 GetALHandle() const
	{
		return alHandle;
	}

	/**
	 * Is muted audio source
	 * @return Return TRUE if audio source is muted, else return FALSE
	 */
	FORCEINLINE bool IsMuted() const
	{
		return bMuted;
	}

protected:
	TAssetHandle<CAudioBank>					audioBank;				/**< Audio bank */
	float										volume;					/**< Volume */

#if WITH_EDITOR
	COnAudioBankUpdated::DelegateType_t*		audioBankUpdatedHandle;	/**< Handle of delegate of updated audio bank */
#endif // WITH_EDITOR

private:
	/**
	 * On audio device muted/unmuted
	 * @param InIsAudioDeviceMuted Is audio device muted
	 */
	void OnAudioDeviceMuted( bool InIsAudioDeviceMuted );

	/**
	 * On audio buffer is destroyed
	 * @param InAudioBuffer		Audio buffer
	 */
	void OnAudioBufferDestroyed( class CAudioBuffer* InAudioBuffer );

	/**
	 * On audio buffer is updated
	 * @param InAudioBuffer		Audio buffer
	 */
	void OnAudioBufferUpdated( class CAudioBuffer* InAudioBuffer );

#if WITH_EDITOR
	/**
	 * On audio bank is updated
	 * @param InAudioBank		Audio bank
	 */
	void OnAudioBankUpdated( class CAudioBank* InAudioBank );
#endif // WITH_EDITOR

	bool										bMuted;						/**< Is audio source muted */
	uint32										alHandle;					/**< OpenAL of sound source */
	COnAudioDeviceMuted::DelegateType_t*		audioDeviceMutedHandle;		/**< Handle of delegate of muted device */
	COnAudioBufferDestroyed::DelegateType_t*	audioBufferDestroyedHandle;	/**< Handle of delegate of destroyed audio buffer */
	COnAudioBufferUpdated::DelegateType_t*		audioBufferUpdatedHandle;	/**< Handle of delegate of updated audio buffer */
};

#endif // !AUDIOSOURCE_H