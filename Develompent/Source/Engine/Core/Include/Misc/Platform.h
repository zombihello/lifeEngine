/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PLATFORM_H
#define PLATFORM_H

// By default all defines PLATFORM_XXX and PLATFORM_USE_XXX is 0
#define PLATFORM_WINDOWS		            0
#define PLATFORM_USE__ALIGNED_MALLOC        0
#define PLATFORM_IS_STD_MALLOC_THREADSAFE   0
#define PLATFORM_SUPPORTS_MIMALLOC          0

// Platform specific definitions
#if _WIN32 || _WIN64        // Windows platform
    #include "WindowsPlatform.h"
#else                       // Unknown platform
    #error Unknown platform
#endif // _WIN32 || _WIN64

#if DOXYGEN
    #define PLATFORM_DOXYGEN        1
#else
    #define PLATFORM_DOXYGEN        0
#endif // DOXYGEN

#endif // !PLATFORM_H
