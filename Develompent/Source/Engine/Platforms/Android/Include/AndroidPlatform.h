/**
 * @file
 * @addtogroup AndroidPlatform Android platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/24/2021
 */

#ifndef LIFEENGINE_ANDROIDPLATFORM_H
#define LIFEENGINE_ANDROIDPLATFORM_H

#include <stdarg.h>

#include "Types.h"

#define PLATFORM_ANDROID					    1

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define appIsDebuggerPresent()	            false
    #define appDebugBreak()
#else
    /**
     * @ingroup AndroidPlatform
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define SHIPPING_BUILD this macro is always return false
     */
    #define appIsDebuggerPresent()	             true

    /**
    * @ingroup AndroidPlatform
    * @brief Macro for for triggering breakpoint
    * @warning With enabled define SHIPPING_BUILD this macro is empty
    */
    #define appDebugBreak()			             ( *( ( int* )3 ) = 13 )
#endif // SHIPPING_BUILD

/**
 * @ingroup AndroidPlatform
 * @brief Calling convention. Functions with variable arguments
 */
#define VARARGS

/**
 * @ingroup AndroidPlatform
 * @brief Calling convention. Standard C function
 */
#define CDECL

/**
 * @ingroup AndroidPlatform
 * @brief Calling convention. Standard calling convention
 */
#define STDCALL

/**
 * @ingroup AndroidPlatform
 * @brief Force code to be inline
 */
#define FORCEINLINE			inline __attribute__ ( ( always_inline ) )

/**
 * @ingroup AndroidPlatform
 * @brief Force code to NOT be inline
 */
#define FORCENOINLINE		__attribute__( ( noinline ) )

/**
 * @ingroup AndroidPlatform
 * @brief Export from DLL
 */
#define DLLEXPORT			__attribute__( ( visibility( "default" ) ) )

/**
 * @ingroup AndroidPlatform
 * @brief Import to DLL
 */
#define DLLIMPORT			__attribute__( ( visibility( "default" ) ) )

/**
 * @ingroup AndroidPlatform
 * @brief JNI Method
 */
#define JNI_METHOD			__attribute__( ( visibility( "default" ) ) ) extern "C"

/**
 * @ingroup AndroidPlatform
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
 * @ingroup AndroidPlatform
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

#endif //LIFEENGINE_ANDROIDPLATFORM_H
