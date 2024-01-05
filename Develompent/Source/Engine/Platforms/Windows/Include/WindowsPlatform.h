/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSPLATFORM_H
#define WINDOWSPLATFORM_H

#include <Windows.h>
#include <tchar.h>
#include <stdarg.h>

#include "Misc/Types.h"

#define PLATFORM_WINDOWS					        1

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

#undef GetObject

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

/**
 * @ingroup WindowsPlatform
 * @brief Get string length
 * 
 * @param InString      String
 * @return Return string length
 */
FORCEINLINE uint32 Sys_Strlen( const achar* InString ) { return strlen( InString ); }

/**
 * @ingroup WindowsPlatform
 * @brief Get string length
 *
 * @param InString      String
 * @return Return string length
 */
FORCEINLINE uint32 Sys_Strlen( const tchar* InString ) { return _tcslen( InString ); }

/**
 * @ingroup WindowsPlatform
 * @brief Compare strings without case sensitivity
 * 
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 Sys_Stricmp( const achar* InString1, const achar* InString2 ) { return _stricmp( InString1, InString2 ); }

/**
 * @ingroup WindowsPlatform
 * @brief Compare strings without case sensitivity
 *
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @return Return a value indicating the relationship between the two strings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 Sys_Stricmp( const tchar* InString1, const tchar* InString2 ) { return _tcsicmp( InString1, InString2 ); }

/**
 * @ingroup WindowsPlatform
 * @brief Compares the specified number of characters of two strings without regard to case
 * 
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @param InCount       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 Sys_Strnicmp( const achar* InString1, const achar* InString2, uint32 InCount ) { return _strnicmp( InString1, InString2, InCount ); }

/**
 * @ingroup WindowsPlatform
 * @brief Compares the specified number of characters of two strings without regard to case
 *
 * @param InString1     String 1 to compare
 * @param InString2     String 2 to compare
 * @param InCount       Number of characters to compare
 * @return Return a value indicating the relationship between the substrings, as follows: Less than 0 - InString1 less than InString2; 0 - InString1 equivalent to InString2; Greater than 0 - InString1 greater than InString2
 */
FORCEINLINE uint32 Sys_Strnicmp( const tchar* InString1, const tchar* InString2, uint32 InCount ) { return _tcsnicmp( InString1, InString2, InCount ); }

/**
 * @ingroup WindowsPlatform
 * @brief Convert string to integer
 * 
 * @param InString      String to convert
 * @return Return converted string to integer
 */
FORCEINLINE uint32 Sys_Atoi( const tchar* InString ) { return _tstoi( InString ); }

#endif // !WINDOWSPLATFORM_H
