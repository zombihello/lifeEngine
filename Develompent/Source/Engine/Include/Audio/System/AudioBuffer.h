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

#ifndef AUDIOBUFFER_H
#define AUDIOBUFFER_H

#include <AL/al.h>
#include <AL/alc.h>

#include "Core/Misc/RefCounted.h"
#include "Core/Misc/RefCountPtr.h"
#include "Audio/System/AudioDevice.h"
#include "Core/System/Delegate.h"

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