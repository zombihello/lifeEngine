/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TYPES_H
#define TYPES_H

#include "LEBuild.h"

/**
 * @ingroup Core Core
 * @brief Signed int 8-bit
 */
typedef	signed char			int8;

/**
 * @ingroup Core Core
 * @brief Signed int 16-bit
 */
typedef	short				int16;

/**
 * @ingroup Core Core
 * @brief Signed int 32-bit
 */
typedef	int					int32;

/**
 * @ingroup Core Core
 * @brief Signed int 64-bit
 */
typedef	long long			int64;

/**
 * @ingroup Core Core
 * @brief Unsigned int 8-bit
 */
typedef	unsigned char		uint8;

/**
 * @ingroup Core Core
 * @brief Unsigned int 16-bit
 */
typedef	unsigned short		uint16;

/**
 * @ingroup Core Core
 * @brief Unsigned int 32-bit
 */
typedef	unsigned int		uint32;

/**
 * @ingroup Core Core
 * @brief Unsigned int 64-bit
 */
typedef	unsigned long long	uint64;

/**
 * @ingroup Core Core
 * @brief Unsigned int 8-bit
 */
typedef	uint8		        byte;

/**
 * @ingroup Core Core
 * @brief ANSI char
 */
typedef char				achar;

/**
 * @ingroup Core Core
 * @brief Unicode char
 */
typedef wchar_t				tchar;

#if PLATFORM_64BIT
    /**
     * @ingroup Core Core
     * @brief Pointer type
     */
    typedef int64           ptrint;

    /**
     * @ingroup Core Core
     * @brief Unsigned pointer type
     */
    typedef uint64          uptrint;
#elif PLATFORM_32BIT
    typedef int32           ptrint;
    typedef uint32          uptrint;
#else
#endif // PLATFORM_64BIT

#endif // !TYPES_H
