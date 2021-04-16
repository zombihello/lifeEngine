/**
 * @file
 * @addtogroup Core Core
 *
 * @authors: Yehor Pohuliaka
 * @date: 3/23/2021
 */

#ifndef CORE_H
#define CORE_H

#include "LEBuild.h"
#include "CoreDefines.h"
#include "Logger/LoggerMacros.h"
#include "Misc/Types.h"
#include "Misc/Globals.h"

#undef TEXT

/**
 * @ingroup Core
 * @brief Macro for convert string to WChar
 *
 * @param[in] String Input string
 *
 *  Example usage: @code TEXT( "Hello World" ) @endcode
 */
#define TEXT( String )      L##String

 /**
  * @ingroup Core
  * @brief Failed assertion handler
  *
  * @param[in] InExpr Condition that was not met
  * @param[in] InFile File path with error
  * @param[in] InLine Error line
  * @param[in] InFormat Displayed message format
  * @param[in] ... Message parameters
  *
  * Example usage: @code appFailAssertFunc( "Value != 0", __FILE__, __LINE__, TEXT( "Value = %i" ), Value ); @endcode
  */
void VARARGS            appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... );

/**
 * @ingroup Core
 * @brief Failed assertion handler debug version
 *
 * @param[in] InExpr Condition that was not met
 * @param[in] InFile File path with error
 * @param[in] InLine Error line
 * @param[in] InFormat Displayed message format
 * @param[in] ... Message parameters
 *
 * Example usage: @code appFailAssertFuncDebug( "Value != 0", __FILE__, __LINE__, TEXT( "Value = %i" ), Value ); @endcode
 */
void VARARGS            appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... );

/**
 * @ingroup Core
 * @brief Get formatted string (for Unicode strings)
 *
 * @param[in,out] InOutDest Pointer to destination buffer
 * @param[in] InDestSize Size of destination buffer
 * @param[in] InCount umber of characters to write (not including null terminating character)
 * @param[in] InFormat String to print
 * @param[in] InArgPtr Argument list
 * @return Number of characters written or -1 if truncated
 */
extern int              appGetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr );

/**
 * @ingroup Core
 * @brief Get formatted string (for ANSI strings)
 *
 * @param[in,out] InOutDest Pointer to destination buffer
 * @param[in] InDestSize Size of destination buffer
 * @param[in] InCount umber of characters to write (not including null terminating character)
 * @param[in] InFormat String to print
 * @param[in] InArgPtr Argument list
 * @return Number of characters written or -1 if truncated
 */
extern int              appGetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr );

/**
 * @ingroup Core
 * @brief Macros for call fail assert
 *
 * @param[in] Expr Condition that was not met
 * @param[in] File File path with error
 * @param[in] Line Error line
 * @param[in] ... Message parameters
 *
 * Example usage: @code appFailAssert( Value != 0, __FILE__, __LINE__, TEXT( "Value = %i" ), Value ) @endcode
 */
#define appFailAssert( Expr, File, Line, ... )				{ if ( appIsDebuggerPresent() )  { appFailAssertFuncDebug( Expr, File, Line, ##__VA_ARGS__ ); } appDebugBreak(); appFailAssertFunc( Expr, File, Line, ##__VA_ARGS__ ); }

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define appErrorf( ... )
#else
    /**
    * @ingroup Core
    * @brief Macro for calling and exiting a program error
    * @warning With enabled define SHIPPING_BUILD this macro is empty
    *
    * @param[in] ... Message parameters
    *
    * Example usage: @code appErrorf( TEXT( "Failed :(" ) ) @endcode
    */
    #define appErrorf( ... )				( ( appIsDebuggerPresent() ? appFailAssertFuncDebug( "appErrorf", __FILE__, __LINE__, ##__VA_ARGS__ ), 1 : 1 ), appDebugBreak(), 1 )
#endif // SHIPPING_BUILD

#if DO_CHECK || PLATFORM_DOXYGEN
    /**
    * @ingroup Core
    * @brief Macro for checking the condition with your code
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * @param[in] Code Verification code
    *
    * Example usage: @code checkCode( if( !isValid ) appErrorf( TEXT( "Value not valid" ) ) ) @endcode
    */
    #define checkCode( Code )		        do { Code } while ( false );

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, display your message
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * @param[in] Expr Checkable condition
    * @param[in] Msg Message
    *
    * Example usage: @code checkMsg( Value == 0, TEXT( "Value = %i" ), Value ) @endcode
    */
    #define checkMsg( Expr, Msg )		    { if ( !( Expr ) ) { appFailAssert( #Expr " : " #Msg , __FILE__, __LINE__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, call your function
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * @param[in] Expr Checkable condition
    * @param[in] Func Called function
    *
    * Example usage: @code checkFunc( Value == 0, MyFunction() ) @endcode
    */
    #define checkFunc( Expr, Func )	        { if ( !( Expr ) ) { Func; appFailAssert( #Expr, __FILE__, __LINE__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * @param[in] Expr Checkable condition
    *
    * Example usage: @code check( Value == 0 ) @endcode
    */
    #define check( Expr )				    { if ( !( Expr ) ) { appFailAssert( #Expr, __FILE__, __LINE__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for triggering an error if a prohibited function is entered
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * Example usage:
     * @code
     * void Function()
     * {
     *      checkNoEntry()
     * }
     *
     * int main()
     * {
     *      Function();     // Error
     * }
     * @endcode
    */
    #define checkNoEntry()                  { appFailAssert( "Enclosing block should never be called", __FILE__, __LINE__ ); }

    /**
    * @ingroup Core
    * @brief Macro to handle the case for a single entry into a function
    * @warning With enabled define DO_CHECK this macro is empty
    *
    * Example usage:
    * @code
    * void Function()
    * {
    *      checkNoReentry()
    * }
    *
    * int main()
    * {
    *      Function();
    *      Function();     // Error
    * }
    * @endcode
    */
    #define checkNoReentry()                {   static bool s_beenHere##__LINE__ = false;                                       \
                                                checkMsg( !s_beenHere##__LINE__, Enclosing block was called more than once );   \
                                                s_beenHere##__LINE__ = true; }
#else
    #define checkCode( Code )		        {}
    #define checkMsg( Expr, Msg )		    {}
    #define checkFunc( Expr, Func )	        {}
    #define verify( Expr )			        {}
    #define check( Expr )				    {}
    #define checkNoEntry()                  {}
    #define checkNoReentry()                {}
#endif // DO_CHECK

#endif //CORE_H
