/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/22/2021
 */

#ifndef WINDOWSPLATFORM_H
#define WINDOWSPLATFORM_H

#include <Windows.h>
#include <stdarg.h>

#include "Misc/Types.h"

#define PLATFORM_WINDOWS					        1

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define appIsDebuggerPresent()	                false
    #define appDebugBreak()
#else
    /**
     * @ingroup WindowsPlatform
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define SHIPPING_BUILD this macro is always return false
     */
    #define appIsDebuggerPresent                    IsDebuggerPresent

    /**
    * @ingroup WindowsPlatform
    * @brief Macro for for triggering breakpoint
    * @warning With enabled define SHIPPING_BUILD this macro is empty
    */
    #define appDebugBreak()			                ( appIsDebuggerPresent() ? ( DebugBreak(), 1 ) : 1 )
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
  * @brief Get formatted string (for Unicode strings)
  *
  * @param[in,out] InOutDest Pointer to destination buffer
  * @param[in] InDestSize Size of destination buffer
  * @param[in] InCount umber of characters to write (not including null terminating character)
  * @param[in] InFormat String to print
  * @param[in] InArgPtr Argument list
  * @return Number of characters written or -1 if truncated
  */
int appGetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr );

/**
 * @ingroup WindowsPlatform
 * @brief Get formatted string (for ANSI strings)
 *
 * @param[in,out] InOutDest Pointer to destination buffer
 * @param[in] InDestSize Size of destination buffer
 * @param[in] InCount umber of characters to write (not including null terminating character)
 * @param[in] InFormat String to print
 * @param[in] InArgPtr Argument list
 * @return Number of characters written or -1 if truncated
 */
int appGetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr );

#endif // !WINDOWSPLATFORM_H
