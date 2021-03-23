/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/22/2021
 */

#ifndef LIFEENGINE_WINDOWSPLATFORM_H
#define LIFEENGINE_WINDOWSPLATFORM_H

#define PLATFORM_WINDOWS					    1

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define appIsDebuggerPresent	                false
    #define appDebugBreak()
#else
    /**
     * @ingroup WindowsPlatform
     * @brief Macro for checking the presence of a debugger
     * @warning With enabled define SHIPPING_BUILD this macro is always return false
     */
    #define appIsDebuggerPresent	                IsDebuggerPresent

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

#endif //LIFEENGINE_WINDOWSPLATFORM_H
