/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <AL/al.h>
#include <AL/alc.h>
#include <string>

#include "System/Archive.h"
#include "Misc/Types.h"
#include "Math/Math.h"

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
 * @brief Convert sample format from ESampleFormat to OpenAL
 * 
 * @param InSampleFormat Sample format
 * @return Return format in OpenAL
 */
uint32 appSampleFormatToEngine( ESampleFormat InSampleFormat );

/**
 * @ingroup Audio
 * @brief Struct of descriptrion listener spatial
 */
struct FListenerSpatial
{
	/**
	 * @brief Constructor
	 */
	FListenerSpatial()
		: location( FMath::vectorZero )
		, forward( FMath::vectorForward )
		, up( FMath::vectorUp )
	{}

	FVector		location;	/**< Location in world */
	FVector		forward;	/**< Forward vector */
	FVector		up;			/**< Up vector */
};

/**
 * @ingroup Audio
 * @brief Class of audio device
 */
class FAudioDevice
{
public:
	/**
	 * @brief Constructor
	 */
	FAudioDevice();

	/**
	 * @brief Destructor
	 */
	~FAudioDevice();

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
	FORCEINLINE void SetListenerSpatial( const FVector& InLocation, const FVector& InForward, const FVector& InUp )
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
		alListenerf( AL_GAIN, InVolume * 0.01f );
		globalVolume = InVolume;
	}

	/**
	 * @brief Get listener spatial
	 * @return Return listener spatial
	 */
	FORCEINLINE const FListenerSpatial& GetListenerSpatial() const
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

private:
	class ALCdevice*		alDevice;				/**< OpenAL device */
	class ALCcontext*		alContext;				/**< OpenAL context */
	FListenerSpatial		listener;				/**< Listener */
	float					globalVolume;			/**< Global volume */
	float					platformAudioHeadroom;	/**< Defines a platform-specific volume headroom (in dB) for audio to provide better platform consistency with respect to volume levels */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, ESampleFormat& InValue )
{
	InArchive.Serialize( &InValue, sizeof( ESampleFormat ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ESampleFormat& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( byte* ) &InValue, sizeof( ESampleFormat ) );
	return InArchive;
}

#endif // !AUDIODEVICE_H