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

#ifndef DEBUG_H
#define DEBUG_H

#include "core/core.h"

/**
 * @ingroup core
 * @brief Type of externally defined function used to display messages
 */
typedef void ( *LogOutputFn_t )( const achar* pMsg );

/**
* @ingroup core
* @brief Set output function for display messages
*
* @param pFunc    Pointer to function
*/
CORE_INTERFACE void Sys_SetLogOutputFunc( LogOutputFn_t pFunc );

/**
 * @ingroup core
 * @brief Get current output function for display messages
 * @return Return pointer to output function
 */
CORE_INTERFACE LogOutputFn_t Sys_GetLogOutputFunc();

/**
 * @ingroup core
 * @brief Get default output function
 * @return Return pointer to default output function
 */
CORE_INTERFACE LogOutputFn_t Sys_GetDefaultLogOutput();

/**
 * @ingroup core
 * @brief Setup console IO for logs
 * @note Need implement on each platform
 */
CORE_INTERFACE void Sys_SetupConsoleIO();

/**
 * @ingroup core
 * @brief Is console IO was initialized for logs
 * @return Return TRUE if console IO was initialized for logs, otherwise returns FALSE
 */
CORE_INTERFACE bool Sys_IsInitedConsoleIO();

#if ENABLE_LOGGING
    /**
     * @ingroup core
     * @brief Print message to log
     *
     * @param pFormat   Message format
     */
    CORE_INTERFACE void Msg( const achar* pFormat, ... );

    /**
	 * @ingroup core
	 * @brief Print message to log
	 *
	 * @param pFormat   Message format
     * @param params    Other message parameters
	 */
    CORE_INTERFACE void VMsg( const achar* pFormat, va_list params );

    /**
     * @ingroup core
     * @brief Print warning message to log
     *
     * @param pFormat    Message format
     */
    CORE_INTERFACE void Warning( const achar* pFormat, ... );

    /**
     * @ingroup core
     * @brief Print warning message to log
     *
     * @param pFormat  Message format
     * @param params   Other message parameters
     */
    CORE_INTERFACE void VWarning( const achar* pFormat, va_list params );

    /**
     * @ingroup core
     * @brief Print error message to log
     *
     * @param pFormat   Message format
     */
    CORE_INTERFACE void Error( const achar* pFormat, ... );

    /**
     * @ingroup core
     * @brief Print error message to log
     *
     * @param pFormat   Message format
     * @param params    Other message parameters
     */
    CORE_INTERFACE void VError( const achar* pFormat, va_list params );
#else
    FORCEINLINE void Msg( const achar* pFormat, ... )                   {}
    FORCEINLINE void VMsg( const achar* pFormat, va_list params )       {}
    FORCEINLINE void Warning( const achar* pFormat, ... )               {}
    FORCEINLINE void VWarning( const achar* pFormat, va_list params )   {}
    FORCEINLINE void Error( const achar* pFormat, ... )                 {}
    FORCEINLINE void VError( const achar* pFormat, va_list params )     {}
#endif // ENABLE_LOGGING

#if ENABLE_ASSERT
    /**
     * @ingroup core
     * @brief Failed assertion handler
     *
     * @param pExpr		Condition that was not met
     * @param pFile		File path with error
     * @param line		Error line
     * @param pFormat	Display message format
     * @param ...		Message parameters
     *
     * Example usage: @code Sys_FailAssertFunc( "Value != 0", __FILE__, __LINE__, "Value = %i", Value ); @endcode
     */
    CORE_INTERFACE void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... );

    /**
    * @ingroup core
    * @brief Macro for checking the condition with your code
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * @param Code   Verification code
    *
    * Example usage: @code AssertCode( if( !isValid ) Sys_Error( "Value not valid" ) ) @endcode
    */
    #define AssertCode( Code ) \
        do \
        { \
            Code \
        } \
        while ( false );

    /**
    * @ingroup core
    * @brief Macro for checking the condition and in case of failure, display your message
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * @param Expr   Checkable condition
    * @param Msg    Message
    *
    * Example usage: @code AssertMsg( Value == 0, "Value = %i", Value ) @endcode
    */
    #define AssertMsg( Expr, Msg, ... )	\
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ); \
        } \
    }

    /**
    * @ingroup core
    * @brief Macro for checking the condition and in case of failure, call your function
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * @param Expr   Checkable condition
    * @param Func   Called function
    *
    * Example usage: @code AssertFunc( Value == 0, MyFunction() ) @endcode
    */
    #define AssertFunc( Expr, Func ) \
    { \
        if ( !( Expr ) ) \
        { \
            Func; \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__ ); \
        } \
    }

    /**
    * @ingroup core
    * @brief Macro for checking the condition
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * @param Expr   Checkable condition
    *
    * Example usage: @code Assert( Value == 0 ) @endcode
    */
    #define Assert( Expr ) \
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( #Expr, __FILE__, __LINE__ ); \
        } \
    }

    /**
    * @ingroup core
    * @brief Macro for triggering an error if a prohibited function is entered
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * Example usage:
     * @code
     * void Function()
     * {
     *      AssertNoEntry()
     * }
     *
     * int main()
     * {
     *      Function();     // Error
     * }
     * @endcode
    */
    #define AssertNoEntry()                     Sys_FailAssertFunc( "Enclosing block should never be called", __FILE__, __LINE__ );

    /**
    * @ingroup core
    * @brief Macro to handle the case for a single entry into a function
    * @warning With disabled define ENABLE_ASSERT this macro is empty
    *
    * Example usage:
    * @code
    * void Function()
    * {
    *      AssertNoReentry()
    * }
    *
    * int main()
    * {
    *      Function();
    *      Function();     // Error
    * }
    * @endcode
    */
    #define AssertNoReentry() \
    { \
        static bool s_beenHere##__LINE__ = false; \
        AssertMsg( !s_beenHere##__LINE__, "Enclosing block was called more than once" ); \
        s_beenHere##__LINE__ = true; \
    }
#else
    FORCEINLINE void Sys_FailAssertFunc( const achar* pExpr, const achar* pFile, int32 line, const achar* pFormat = "", ... ) {}
    #define AssertCode( Code )		        {}
    #define AssertMsg( Expr, Msg, ... )		{}
    #define AssertFunc( Expr, Func )	    {}
    #define Assert( Expr )				    {}
    #define AssertNoEntry()                 {}
    #define AssertNoReentry()               {}
#endif // ENABLE_ASSERT

#endif // !DEBUG_H