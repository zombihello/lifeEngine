/**
 * @file
 * @addtogroup Core Core
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

#ifndef COREDEFINES_H
#define COREDEFINES_H

// By default all defines PLATFORM_XXX is 0
#define PLATFORM_WINDOWS		0

// Platform specific definitions
#if _WIN32 || _WIN64        // Windows platform
    #include "Core/Platforms/Windows/WindowsPlatform.h"
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
