/**
 * @file
 * @addtogroup AndroidPlatform Android platform
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/24/2021
 */

#ifndef LIFEENGINE_ANDROIDPLATFORM_H
#define LIFEENGINE_ANDROIDPLATFORM_H

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

#endif //LIFEENGINE_ANDROIDPLATFORM_H
