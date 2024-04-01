/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSMEMORY_H
#define WINDOWSMEMORY_H

#include "System/GenericPlatformMemory.h"

/**
 * @ingroup WindowsPlatform
 * @brief Windows implementation of the memory OS functions
 */
struct WindowsPlatformMemory : public GenericPlatformMemory
{
	/**
	 * @brief Allocate the default allocator for current platform
	 * @return Return allocated the default allocator for current platform
	 */
	static CBaseMalloc* AllocDefaultAllocator();
};

/**
 * @ingroup WindowsPlatform
 * @brief Typedef of Windows platform memory
 */
typedef WindowsPlatformMemory		PlatformMemory;

#endif // !WINDOWSMEMORY_H