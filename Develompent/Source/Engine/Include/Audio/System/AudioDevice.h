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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

#include "Core/System/Archive.h"
#include "Core/Misc/Types.h"
#include "Core/Math/Math.h"
#include "Core/System/Delegate.h"

/** Delegate called when audio device muted/unmuted */
DECLARE_MULTICAST_DELEGATE( COnAudioDeviceMuted, bool )

/**
 * @ingroup Audio
 * @brief Enumeration of sample format type
 */
enum ESampleFormat
{
	SF_Mono8,			/**< Mono 8-bit */
	SF_Mono16,			/**< Mono 16-bit */
	SF_Stereo8,			/**< Stereo 8-bit */
	SF_Stereo16,		/**< Stereo 16-bit */
	SF_Unknown			/**< Unknown */
};

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
 * @brief Convert sample format from ESampleFormat to OpenAL
 * 
 * @param InSampleFormat Sample format
 * @return Return format in OpenAL
 */
uint32 Sys_SampleFormatToEngine( ESampleFormat InSampleFormat );

/**
 * @ingroup Audio
 * @brief Convert sample format from ESampleFormat to text
 * 
 * @param InSampleFormat	Sample format
 * @return Return text sampler format
 */
std::wstring Sys_SampleFormatToText( ESampleFormat InSampleFormat );

/**
 * @ingroup Audio
 * @brief Get number bytes for one sample
 * 
 * @param InSampleFormat	Sample format
 * @return Return number bytes for one sample
 */
uint32 Sys_GetNumSampleBytes( ESampleFormat InSampleFormat );

/**
 * @ingroup Audio
 * @brief Struct of descriptrion listener spatial
 */
struct ListenerSpatial
{
	/**
	 * @brief Constructor
	 */
	ListenerSpatial()
		: location( Math::vectorZero )
		, forward( Math::vectorForward )
		, up( Math::vectorUp )
	{}

	Vector		location;	/**< Location in world */
	Vector		forward;	/**< Forward vector */
	Vector		up;			/**< Up vector */
};

/**
 * @ingroup Audio
 * @brief Class of audio device
 */
class CAudioDevice
{
public:
	/**
	 * @brief Constructor
	 */
	CAudioDevice();

	/**
	 * @brief Destructor
	 */
	~CAudioDevice();

	/**
	 * @brief Initialize engine
	 */
	void Init();

	/**
	 * @brief Shutdown engine
	 */
	void Shutdown();

	/**
	 * @brief Set listener spatial
	 * 
	 * @param InLocation Location in world
	 * @param InForward Forward vector
	 * @param InUp Up vector
	 */
	FORCEINLINE void SetListenerSpatial( const Vector& InLocation, const Vector& InForward, const Vector& InUp )
	{
		listener.location = InLocation;
		listener.forward = InForward;
		listener.up = InUp;

		float		orientation[] = { InForward.x, InForward.y, InForward.z, InUp.x, InUp.y, InUp.z };
		alListenerfv( AL_ORIENTATION, orientation );
		alListener3f( AL_POSITION, InLocation.x, InLocation.y, InLocation.z );
	}
	
	/**
	 * @brief Set global volume
	 * 
	 * @param InVolume Volume
	 */
	FORCEINLINE void SetGlobalVolume( float InVolume )
	{
		alListenerf( AL_GAIN, InVolume * platformAudioHeadroom );
		globalVolume = InVolume;
	}

	/**
	 * @brief Set mute device
	 * @param InIsMutedDevice Is need mute device
	 */
	FORCEINLINE void SetMutedDevice( bool InIsMutedDevice )
	{
		bIsMuted = InIsMutedDevice;
		onAudioDeviceMuted.Broadcast( InIsMutedDevice );
	}

	/**
	 * @brief Get listener spatial
	 * @return Return listener spatial
	 */
	FORCEINLINE const ListenerSpatial& GetListenerSpatial() const
	{
		return listener;
	}
	
	/**
	 * @brief Get global volume
	 * @return Return global volume
	 */
	FORCEINLINE float GetGlobalVolume() const
	{
		return globalVolume;
	}

	/**
	 * @brief Is OpenAL supported extension
	 *
	 * @param InExtension Interested extension OpenAL
	 * @return Return true if extension is supported, else return false
	 */
	bool IsExtensionSupported( const std::string& InExtension ) const;

	/**
	 * @brief Get platform audio headroom
	 * @return Return platform audio headroom
	 */
	FORCEINLINE float GetPlatformAudioHeadroom() const
	{
		return platformAudioHeadroom;
	}

	/**
	 * @brief Is muted device
	 * @return Return true if device is muted, else return false
	 */
	FORCEINLINE bool IsMutedDevice() const
	{
		return bIsMuted;
	}

	/**
	 * @brief Get delegate of audio device muted/unmute
	 * @return Return delegate of audio device muted/unmute
	 */
	FORCEINLINE COnAudioDeviceMuted& OnAudioDeviceMuted() const
	{
		return onAudioDeviceMuted;
	}

private:
	bool							bIsMuted;				/**< Is muted device */
	class ALCdevice*				alDevice;				/**< OpenAL device */
	class ALCcontext*				alContext;				/**< OpenAL context */
	ListenerSpatial				listener;				/**< Listener */
	float							globalVolume;			/**< Global volume */
	float							platformAudioHeadroom;	/**< Defines a platform-specific volume headroom (in dB) for audio to provide better platform consistency with respect to volume levels */
	mutable COnAudioDeviceMuted		onAudioDeviceMuted;		/**< Delegate called when audio device muted/unmuted */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, ESampleFormat& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ESampleFormat ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ESampleFormat& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( byte* ) &InValue, sizeof( ESampleFormat ) );
	return InArchive;
}

#endif // !AUDIODEVICE_H