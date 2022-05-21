/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBUFFERMANAGER_H
#define AUDIOBUFFERMANAGER_H

#include <unordered_map>

#include "System/AudioBank.h"
#include "System/AudioBuffer.h"

/**
 * @ingroup Audio
 * @brief Class for manager loaded fully banks to memory
 */
class FAudioBufferManager
{
public:
	/**
	 * Find loaded buffer by bank
	 * 
	 * @param InAudioBank Audio bank
	 * @return Return finded bank. If failed return nullptr
	 */
	FAudioBufferRef Find( const TSharedPtr<FAudioBank>& InAudioBank );

	/**
	 * Remove loaded buffer
	 * @param InAudioBank Audio bank
	 */
	FORCEINLINE void Remove( const TSharedPtr<FAudioBank>& InAudioBank )
	{
		buffers.erase( InAudioBank );
	}

private:
	std::unordered_map< TWeakPtr<FAudioBank>, FAudioBufferRef, TWeakPtr<FAudioBank>::FHashFunction >		buffers;		/**< Map of fully loaded banks to buffer */
};

#endif // !AUDIOBUFFERMANAGER_H