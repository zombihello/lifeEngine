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
#include "System/Delegate.h"

/**
 * @ingroup Audio
 * @brief Reference to CAudioBuffer
 */
typedef TRefCountPtr< class CAudioBuffer >				AudioBufferRef_t;

/**
 * @ingroup Audio
 * @brief Delegate for called event when OpenAL audio buffer is destroyed
 */
DECLARE_MULTICAST_DELEGATE( COnAudioBufferDestroyed, class CAudioBuffer* );

/**
 * @ingroup Audio
 * @brief Delegate for called event when OpenAL audio buffer is updated
 */
DECLARE_MULTICAST_DELEGATE( COnAudioBufferUpdated, class CAudioBuffer* );

/**
 * @ingroup Audio
 * @brief Audio buffer
 */
class CAudioBuffer : public CRefCounted
{
public:
	/**
	 * Constructor
	 */
	FORCEINLINE CAudioBuffer()
		: alHandle( 0 )
	{}

	/**
	 * Destructor
	 */
	FORCEINLINE ~CAudioBuffer()
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
		alBufferData( alHandle, Sys_SampleFormatToEngine( InSampleFormat ), InSampleData, InSamplesSize, InSampleRate );

		// Tell to all users this buffer what we is updated him
		onAudioBufferUpdated.Broadcast( this );
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

		// Tell to all users this buffer what we destroy OpenAL buffer
		onAudioBufferDestroyed.Broadcast( this );

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

	/**
	 * Get delegate of event when OpenAL audio buffer destroyed
	 * @return Return delegate of event when OpenAL audio buffer destroyed
	 */
	FORCEINLINE COnAudioBufferDestroyed& OnAudioBufferDestroyed() const
	{
		return onAudioBufferDestroyed;
	}

	/**
	 * Get delegate of event when OpenAL audio buffer is updated
	 * @return Return delegate of event when OpenAL audio buffer is updated
	 */
	FORCEINLINE COnAudioBufferUpdated& OnAudioBufferUpdated() const
	{
		return onAudioBufferUpdated;
	}

private:
	uint32								alHandle;					/**< Handle to OpenAL buffer */
	mutable COnAudioBufferDestroyed		onAudioBufferDestroyed;		/**< Delegate of event when OpenAL audio buffer destroyed */
	mutable COnAudioBufferUpdated		onAudioBufferUpdated;		/**< Delegate of event when OpenAL audio buffer is updated */
};

#endif // !AUDIOBUFFER_H