/**
 * @file
 * @addtogroup core core
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

#ifndef TYPES_H
#define TYPES_H

#include "core/build.h"

/**
 * @ingroup core
 * @brief Signed int 8-bit
 */
typedef	signed char			int8;

/**
 * @ingroup core
 * @brief Signed int 16-bit
 */
typedef	short				int16;

/**
 * @ingroup core
 * @brief Signed int 32-bit
 */
typedef	int					int32;

/**
 * @ingroup core
 * @brief Signed int 64-bit
 */
typedef	long long			int64;

/**
 * @ingroup core
 * @brief Unsigned int 8-bit
 */
typedef	unsigned char		uint8;

/**
 * @ingroup core
 * @brief Unsigned int 16-bit
 */
typedef	unsigned short		uint16;

/**
 * @ingroup core
 * @brief Unsigned int 32-bit
 */
typedef	unsigned int		uint32;

/**
 * @ingroup core
 * @brief Unsigned int 64-bit
 */
typedef	unsigned long long	uint64;

/**
 * @ingroup core
 * @brief Unsigned int 8-bit
 */
typedef	uint8		        byte;

/**
 * @ingroup core
 * @brief ANSI char
 */
typedef char				achar;

/**
 * @ingroup core
 * @brief Unicode char
 */
typedef wchar_t				wchar;

/**
 * @ingroup core
 * @brief Hash type
 */
typedef uint64				hash_t;

#if PLATFORM_64BIT
	/**
	 * @ingroup core
	 * @brief Pointer type
	 */
	typedef int64           ptrint;

	/**
	 * @ingroup core
	 * @brief Unsigned pointer type
	 */
	typedef uint64          uptrint;
#elif PLATFORM_32BIT
	typedef int32           ptrint;
	typedef uint32          uptrint;
#else
	#error Unknown platform bit
#endif // PLATFORM_64BIT || PLATFORM_32BIT

#endif // !TYPES_H