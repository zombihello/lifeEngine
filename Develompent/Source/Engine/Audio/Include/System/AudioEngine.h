/**
 * @file
 * @addtogroup Audio Audio
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include "System/AudioDevice.h"

/**
 * @ingroup Audio
 * @brief Main class of audio engine
 */
class ÑAudioEngine
{
public:
	/**
	 * @brief Initialize engine
	 */
	void Init();

	/**
	 * @brief Shutdown engine
	 */
	void Shutdown();
};

#endif // !AUDIOENGINE_H