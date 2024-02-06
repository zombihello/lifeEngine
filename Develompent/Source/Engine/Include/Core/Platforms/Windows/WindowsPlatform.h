/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
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

#ifndef WINDOWSPLATFORM_H
#define WINDOWSPLATFORM_H

#include <Windows.h>
#include <stdarg.h>

#include "COre/Misc/Types.h"

// Mark what we on Windows
#undef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS					        1

// Windows support _aligned_malloc
#undef PLATFORM_USE__ALIGNED_MALLOC
#define PLATFORM_USE__ALIGNED_MALLOC                1

// On Windows ANSI malloc is thread safe
#undef PLATFORM_IS_ANSI_MALLOC_THREADSAFE
#define PLATFORM_IS_ANSI_MALLOC_THREADSAFE          1

// If we on 64 bit platform then it is supports mimalloc
#undef PLATFORM_SUPPORTS_MIMALLOC
#define PLATFORM_SUPPORTS_MIMALLOC                  PLATFORM_64BIT

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define Sys_IsDebuggerPresent()	                false
    #define Sys_DebugBreak()
#else
    /**
     * @ingroup WindowsPlatform
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define SHIPPING_BUILD this macro is always return false
     */
    #define Sys_IsDebuggerPresent                    IsDebuggerPresent

    /**
    * @ingroup WindowsPlatform
    * @brief Macro for for triggering breakpoint
    * @warning With enabled define SHIPPING_BUILD this macro is empty
    */
    #define Sys_DebugBreak()			             ( Sys_IsDebuggerPresent() ? ( DebugBreak(), 1 ) : 1 )
#endif // SHIPPING_BUILD

/**
 * @ingroup WindowsPlatform
 * @brief Calling convention. Functions with variable arguments
 */
#define VARARGS				__cdecl

/**
 * @ingroup WindowsPlatform
 * @brief Calling convention. Standard C function
 */
#undef  CDECL
#define CDECL				__cdecl

/**
 * @ingroup WindowsPlatform
 * @brief Calling convention. Standard calling convention
 */
#define STDCALL				__stdcall

/**
 * @ingroup WindowsPlatform
 * @brief Force code to be inline
 */
#define FORCEINLINE			__forceinline

/**
 * @ingroup WindowsPlatform
 * @brief Force code to NOT be inline
 */
#define FORCENOINLINE		__declspec( noinline )

/**
 * @ingroup WindowsPlatform
 * @brief Export from DLL
 */
#define DLLEXPORT			__declspec( dllexport )

/**
 * @ingroup WindowsPlatform
 * @brief Import to DLL
 */
#define DLLIMPORT			__declspec( dllimport )

/**
 * @ingroup WindowsPlatform
 * @brief Line terminator
 */
#define LINE_TERMINATOR     TEXT( "\n" )

/**
 * @ingroup WindowsPlatform
 * @brief Path separator
 */
#define PATH_SEPARATOR      TEXT( "\\" )

/**
 * @ingroup WindowsPlatform
 * @brief Macro for Assert on char is path separator
 * 
 * @param InCh Char
 */
#define Sys_IsPathSeparator( InCh )	( ( InCh ) == PATH_SEPARATOR[ 0 ] )
 
/**
 * @ingroup WindowsPlatform
 * @brief Align for GCC
 *
 * @param[in] InAlignment Alignment
 */
#define GCC_ALIGN( InAlignment )

/**
 * @ingroup WindowsPlatform
 * @brief Align for Microsoft
 * 
 * @param[in] InAlignment Alignment
 */
#define MS_ALIGN( InAlignment ) __declspec( align( InAlignment ) )

/**
 * @ingroup WindowsPlatform
 * @brief Typedef of window handle
 */
typedef void*           WindowHandle_t;

#endif // !WINDOWSPLATFORM_H
