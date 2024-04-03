/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSPLATFORM_H
#define WINDOWSPLATFORM_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <tchar.h>
#include <stdarg.h>
#include <direct.h>
#include <shellapi.h>

#include "Misc/Types.h"

// Undef some defines
#undef PLATFORM_WINDOWS
#undef PLATFORM_USE__ALIGNED_MALLOC
#undef PLATFORM_IS_STD_MALLOC_THREADSAFE
#undef PLATFORM_SUPPORTS_MIMALLOC
#undef VARARGS
#undef CDECL
#undef STDCALL
#undef FORCEINLINE
#undef FORCENOINLINE
#undef DLLEXPORT
#undef DLLIMPORT
#undef FALSE
#undef TRUE
#undef NULL
#undef GetObject

/** Enable warning C4996 on 1 level (for deprecated messages) */
#pragma warning( 1: 4996 )

// Mark what we on Windows
#define PLATFORM_WINDOWS					        1

// Windows support _aligned_malloc
#define PLATFORM_USE__ALIGNED_MALLOC                1

// On Windows Std malloc is thread safe
#define PLATFORM_IS_STD_MALLOC_THREADSAFE           1

// If we on 64 bit platform then it is supports mimalloc
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
 * @brief True macro
 */
#define TRUE				1

/**
 * @ingroup WindowsPlatform
 * @brief False macro
 */
#define FALSE				0

/**
 * @ingroup WindowsPlatform
 * @brief Null macro
 */
#define NULL				0

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
 * @param InCh      Char
 */
#define Sys_IsPathSeparator( InCh )	    ( ( InCh ) == PATH_SEPARATOR[ 0 ] )
 
/**
 * @ingroup WindowsPlatform
 * @brief Align for GCC
 * @param InAlignment   Alignment
 */
#define GCC_ALIGN( InAlignment )

/**
 * @ingroup WindowsPlatform
 * @brief Align for Microsoft
 * @param InAlignment   Alignment
 */
#define MS_ALIGN( InAlignment )         __declspec( align( InAlignment ) )

/**
 * @ingroup WindowsPlatform
 * @brief Typedef of window handle
 */
typedef void*           WindowHandle_t;

#endif // !WINDOWSPLATFORM_H
