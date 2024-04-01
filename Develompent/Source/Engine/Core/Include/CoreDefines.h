/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef COREDEFINES_H
#define COREDEFINES_H

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

/**
 * @ingroup Core
 * @brief Macro for mark deprecated code
 *
 * @param[in] Version Engine version where this code will be removed
 * @param[in] Message Custom message
 *
 *  Example usage:
 *  @code
 *  LE_DEPRECATED( 1.0.0, "Your message" )
 *  void Function()
 *  {}
 *  @endcode
 */
#define LE_DEPRECATED( Version, Message )			        [ [ deprecated( Message " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile." ) ] ]

/** Enable warning C4996 on 1 level (for deprecated messages) */
#pragma warning( 1: 4996 )

#endif // !COREDEFINES_H
