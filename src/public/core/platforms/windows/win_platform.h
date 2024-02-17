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

#ifndef WIN_PLATFORM_H
#define WIN_PLATFORM_H

#include <Windows.h>

// Undo any defines
#undef PLATFORM_WINDOWS
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

// Mark what we on Windows
#define PLATFORM_WINDOWS	1

#if RETAIL
    #define Sys_IsDebuggerPresent()	                false
    #define Sys_DebugBreak()
    #define Sys_DebugMessage( Msg )
#else
    /**
     * @ingroup core
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define RETAIL this macro is always return FALSE
     */
    #define Sys_IsDebuggerPresent()                 IsDebuggerPresent()

    /**
    * @ingroup core
    * @brief Macro for for triggering breakpoint
    * @warning With enabled define RETAIL this macro is empty
    */
    #define Sys_DebugBreak()			            ( Sys_IsDebuggerPresent() ? ( DebugBreak(), 1 ) : 1 )

    /**
     * @ingroup core
     * @brief Macro for print message to debugger
     * @warning With enabled define RETAIL this macro is empty
     * 
     * @param Msg	    Message
     */
    #define Sys_DebugMessage( Msg )			        OutputDebugStringA( Msg )
#endif // RETAIL

/**
 * @ingroup core
 * @brief DLL extension
 */
#define DLL_EXT_STRING			        ".dll"

// If macro PLATFORM_SUBDIR not defined while compiling, we define it now!
#ifndef PLATFORM_SUBDIR
	#if PLATFORM_64BIT
		/**
		 * @ingroup core
		 * @brief Platform sub directory for binaries
		 */
		#define PLATFORM_SUBDIR		    "win64"
	#elif PLATFORM_32BIT
		#define PLATFORM_SUBDIR		    "win32"
	#else
		#error Unknown platform bit
	#endif // PLATFORM_64BIT || PLATFORM_DOXYGEN
#endif // PLATFORM_SUBDIR

/**
 * @ingroup core
 * @brief Calling convention. Functions with variable arguments
 */
#define VARARGS				            __cdecl

/**
 * @ingroup core
 * @brief Calling convention. Standard C function
 */
#define CDECL				            __cdecl

/**
 * @ingroup core
 * @brief Calling convention. Standard calling convention
 */
#define STDCALL				            __stdcall

/**
 * @ingroup core
 * @brief Force code to be inline
 */
#define FORCEINLINE			            __forceinline

/**
 * @ingroup core
 * @brief Force code to NOT be inline
 */
#define FORCENOINLINE		            __declspec( noinline )

/**
 * @ingroup core
 * @brief Export in DLL
 */
#define DLLEXPORT			            __declspec( dllexport )

/**
 * @ingroup core
 * @brief Import from DLL
 */
#define DLLIMPORT			            __declspec( dllimport )

/**
 * @ingroup core
 * @brief True macro
 */
#define TRUE				            1

/**
 * @ingroup core
 * @brief False macro
 */
#define FALSE				            0

/**
 * @ingroup core
 * @brief Null macro
 */
#define NULL				            0

/**
 * @ingroup core
 * @brief Line terminator
 */
#define LINE_TERMINATOR                 "\n"

/**
 * @ingroup core
 * @brief Path separator
 */
#define PATH_SEPARATOR                  "\\"
 
/**
 * @ingroup core
 * @brief Align for GCC
 *
 * @param Alignment     Alignment
 */
#define GCC_ALIGN( Alignment )

/**
 * @ingroup core
 * @brief Align for Microsoft
 * 
 * @param Alignment     Alignment
 */
#define MS_ALIGN( Alignment )           __declspec( align( Alignment ) )

/**
 * @ingroup core
 * @brief Invalid thread handle
 */
#define INVALID_THREAD_HANDLE           nullptr

/**
 * @ingroup core
 * @brief DLL handle
 */
typedef	HMODULE	                        dllHandle_t;

/**
 * @ingroup core
 * @brief Window handle
 */
typedef HANDLE                          windowHandle_t;

/**
 * @ingroup core
 * @brief Thread handle
 */
typedef HANDLE                          threadHandle_t;

#endif // !WIN_PLATFORM_H