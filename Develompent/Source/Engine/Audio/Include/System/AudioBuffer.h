/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <AL/al.h>
#include <AL/alc.h>

#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "System/AudioDevice.h"

/**
 * @ingroup Audio
 * @brief Reference to FAudioBuffer
 */
typedef TRefCountPtr< class FAudioBuffer >				FAudioBufferRef;

/**
 * @ingroup Audio
 * @brief Audio buffer
 */
class FAudioBuffer : public FRefCounted
{
public:
	/**
	 * Constructor
	 */
	FORCEINLINE FAudioBuffer()
		: alHandle( 0 )
	{}

	/**
	 * Destructor
	 */
	FORCEINLINE ~FAudioBuffer()
	{
		Clear();
	}

	/**
	 * Append buffer
	 * 
	 * @param InSampleFormat Sample format
	 * @param InSampleData Array of samples
	 * @param InSamplesSize Size of samples in bytes
	 * @param InSampleRate Rate of sample
	 */
	FORCEINLINE void Append( ESampleFormat InSampleFormat, byte* InSampleData, uint32 InSamplesSize, uint32 InSampleRate )
	{
		if ( alHandle != 0 )
		{
			Clear();
		}

		alGenBuffers( 1, &alHandle );
		alBufferData( alHandle, appSampleFormatToEngine( InSampleFormat ), InSampleData, InSamplesSize, InSampleRate );
	}

	/**
	 * Clear buffer
	 */
	FORCEINLINE void Clear()
	{
		if ( alHandle == 0 )
		{
			return;
		}

		alDeleteBuffers( 1, &alHandle );
		alHandle = 0;
	}

	/**
	 * Get OpenAL handle
	 * @return Return OpenAL handle to buffer. If not created return 0
	 */
	FORCEINLINE uint32 GetALHandle() const
	{
		return alHandle;
	}

private:
	uint32		alHandle;			/**< Handle to OpenAL buffer */
};

#endif // !AUDIOBUFFER_H