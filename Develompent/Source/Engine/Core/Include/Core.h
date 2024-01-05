/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CORE_H
#define CORE_H

#include "LEVersion.h"
#include "LEBuild.h"
#include "CoreDefines.h"
#include "System/MemoryBase.h"
#include "Misc/Template.h"
#include "Misc/Types.h"
#include "Misc/Names.h"
#include "Misc/CoreGlobals.h"

#undef TEXT

/**
 * @ingroup Core
 * Enumeration for no init structs and classes
 */
enum ENoInit
{
    NoInit
};

/**
 * @ingroup Core
 * Invalid hash
 */
enum { INVALID_HASH = ( uint64 )-1 };

/**
 * @ingroup Core
 * Invalid id
 */
enum { INVALID_ID = ( uint32 )-1 };

/**
 * @ingroup Core
 * Invalid index
 */
enum { INDEX_NONE = ( uint32 )-1 };

/**
 * @ingroup Core
 * Platform type
 */
enum EPlatformType
{
    PLATFORM_Unknown,           /**< Unknown platform */
    PLATFORM_Windows            /**< Windows */
};

/**
 * @ingroup Core
 * Message box type
 */
enum EMessageBox
{
    MB_Info,        /**< Info */
    MB_Warning,     /**< Warning */
    MB_Error        /**< Error */
};

/**
 * Returns the string name of the given platform
 *
 * @param InPlatform The platform of interest
 * @return The name of the platform, "" if not found
 */
std::wstring Sys_PlatformTypeToString( EPlatformType InPlatform );

/**
 * Returns the enumeration value for the given platform
 *
 * @param InPlatformStr The platform of interest
 * @return The platform type, or PLATFORM_Unknown if bad input
 */
EPlatformType Sys_PlatformStringToType( const std::wstring& InPlatformStr );

/**
 * @ingroup Core
 * @brief Macro for convert string to WChar
 *
 * @param[in] String Input string
 *
 * Example usage: @code TEXT( "Hello World" ) @endcode
 */
#define TEXT( String )      L##String

/**
 * @ingroup Core
 * @brief Macro for empty code
 */
#define NOOP                ( void )0

/**
 * @ingroup Core
 * @brief Macro for labeling a variable that it is not used
 * 
 * @param[in] Value Value
 * 
 * Example usage: @code UNUSED_VAR( health ); @endcode
 */
#define UNUSED_VAR( Value )     ( void )Value

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
  * Example usage: @code Sys_FailAssertFunc( "Value != 0", __FILE__, __LINE__, TEXT( "Value = %i" ), Value ); @endcode
  */
void VARARGS Sys_FailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... );

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
 * Example usage: @code Sys_FailAssertFuncDebug( "Value != 0", __FILE__, __LINE__, TEXT( "Value = %i" ), Value ); @endcode
 */
void VARARGS Sys_FailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... );

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
int Sys_GetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr );

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
int Sys_GetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr );

/**
 * @ingroup Core
 * @brief Pre-Initialize platform

 * @return Returns the error level, 0 if successful and > 0 if there were errors
 */
int32 Sys_PlatformPreInit();

/**
 * @ingroup Core
 * @brief Initialize platform
 * 
 * @return Returns the error level, 0 if successful and > 0 if there were errors 
 */
int32 Sys_PlatformInit();

/**
 * @ingroup Core
 * @brief Process window events
 */
void Sys_ProcessWindowEvents();

/**
 * @ingroup Core
 * @brief Create process
 * 
 * @param[in] InPathToProcess Executable name
 * @param[in] InParams Command line arguments
 * @param[in] InLaunchDetached If true, the new process will have its own window
 * @param[in] InLaunchReallyHidden If true, the new process will not have a window or be in the task bar
 * @param[in] InPriorityModifier -2 idle, -1 low, 0 normal, 1 high, 2 higher
 * @param[in] OutProcessId if non-NULL, this will be filled in with the ProcessId
 * @return The process handle for use in other process functions
 */
void* Sys_CreateProc( const tchar* InPathToProcess, const tchar* InParams, bool InLaunchDetached, bool InLaunchHidden, bool InLaunchReallyHidden, int32 InPriorityModifier, uint64* OutProcessId = nullptr );

/**
 * @ingroup Core
 * @brief Retrieves the termination status of the specified process
 * 
 * @param[in] InProcHandle Handle of process
 * @param[out] OutReturnCode Return code
 * @return Return true if process is end, else returning false
 */
bool Sys_GetProcReturnCode( void* InProcHandle, int32* OutReturnCode );

/**
 * @ingroup Core
 * @brief Show message box
 * 
 * @param InTitle   Title
 * @param InMessage Message
 * @param InType    Message type
 */
void Sys_ShowMessageBox( const tchar* InTitle, const tchar* InMessage, EMessageBox Intype );

/**
 * @ingroup Core
 * @brief Dump call stack
 * 
 * @param OutCallStack  Output call stack
 */
void Sys_DumpCallStack( std::wstring& OutCallStack );

/**
 * @ingroup Core
 * @brief Request shutdown application
 * 
 * @param InForce   Is need force shutdown application
 */
void Sys_RequestExit( bool InForce );

/**
 * @ingroup Core
 * @brief Is InC a digit
 * 
 * @param InC   Char to check
 * @return Return TRUE if InC is digit, otherwise returns FALSE
 */
FORCEINLINE bool Sys_IsDigit( tchar InC )
{
	return InC >= TEXT( '0' ) && InC <= TEXT( '9' );
}

/**
 * @ingroup Core
 * @brief Macros for call fail assert
 *
 * @param[in] Expr Condition that was not met
 * @param[in] File File path with error
 * @param[in] Line Error line
 * @param[in] ... Message parameters
 *
 * Example usage: @code Sys_FailAssert( Value != 0, __FILE__, __LINE__, TEXT( "Value = %i" ), Value ) @endcode
 */
#define Sys_FailAssert( Expr, File, Line, ... )				{ if ( Sys_IsDebuggerPresent() )  { Sys_FailAssertFuncDebug( Expr, File, Line, ##__VA_ARGS__ ); } Sys_DebugBreak(); Sys_FailAssertFunc( Expr, File, Line, ##__VA_ARGS__ ); }

#if SHIPPING_BUILD && !PLATFORM_DOXYGEN
    #define Sys_Errorf( ... )
#else
    /**
    * @ingroup Core
    * @brief Macro for calling and exiting a program error
    * @warning With enabled define SHIPPING_BUILD this macro is empty
    *
    * @param[in] ... Message parameters
    *
    * Example usage: @code Sys_Errorf( TEXT( "Failed :(" ) ) @endcode
    */
    #define Sys_Errorf( ... )				                ( ( Sys_IsDebuggerPresent() ? Sys_FailAssertFuncDebug( "Sys_Errorf", __FILE__, __LINE__, ##__VA_ARGS__ ), 1 : 1 ), Sys_DebugBreak(), 1 )
#endif // SHIPPING_BUILD

#if ENABLED_ASSERT || PLATFORM_DOXYGEN
    /**
    * @ingroup Core
    * @brief Macro for checking the condition with your code
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param[in] Code Verification code
    *
    * Example usage: @code AssertCode( if( !isValid ) Sys_Errorf( TEXT( "Value not valid" ) ) ) @endcode
    */
    #define AssertCode( Code )		            do { Code } while ( false );

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, display your message
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param[in] Expr Checkable condition
    * @param[in] Msg Message
    *
    * Example usage: @code Assertf( Value == 0, TEXT( "Value = %i" ), Value ) @endcode
    */
    #define Assertf( Expr, Msg, ... )		    { if ( !( Expr ) ) { Sys_FailAssert( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, display your message
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param[in] Expr Checkable condition
    * @param[in] Msg Message
    *
    * Example usage: @code AssertMsg( Value == 0, TEXT( "Value = %i" ), Value ) @endcode
    */
    #define AssertMsg( Expr, Msg, ... )		    { if ( !( Expr ) ) { Sys_FailAssert( #Expr, __FILE__, __LINE__, Msg, __VA_ARGS__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, call your function
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param[in] Expr Checkable condition
    * @param[in] Func Called function
    *
    * Example usage: @code AssertFunc( Value == 0, MyFunction() ) @endcode
    */
    #define AssertFunc( Expr, Func )	        { if ( !( Expr ) ) { Func; Sys_FailAssert( #Expr, __FILE__, __LINE__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param[in] Expr Checkable condition
    *
    * Example usage: @code Assert( Value == 0 ) @endcode
    */
    #define Assert( Expr )				        { if ( !( Expr ) ) { Sys_FailAssert( #Expr, __FILE__, __LINE__ ); } }

    /**
    * @ingroup Core
    * @brief Macro for triggering an error if a prohibited function is entered
    * @warning With enabled define ENABLED_ASSERT this macro is empty
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
    #define AssertNoEntry()                     { Sys_FailAssert( "Enclosing block should never be called", __FILE__, __LINE__ ); }

    /**
    * @ingroup Core
    * @brief Macro to handle the case for a single entry into a function
    * @warning With enabled define ENABLED_ASSERT this macro is empty
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
    #define AssertNoReentry()                {  static bool s_beenHere##__LINE__ = false;                                       \
                                                AssertMsg( !s_beenHere##__LINE__, "Enclosing block was called more than once" );  \
                                                s_beenHere##__LINE__ = true; }
#else
    #define AssertCode( Code )		        {}
    #define Assertf( Expr, Msg, ... )        {}
    #define AssertMsg( Expr, Msg, ... )		{}
    #define AssertFunc( Expr, Func )	        {}
    #define verify( Expr )			        {}
    #define Assert( Expr )				    {}
    #define AssertNoEntry()                  {}
    #define AssertNoReentry()                {}
#endif // ENABLED_ASSERT

// This define must come after the class declaration for CBaseLogger, since the macro uses g_Log->Logf
#if CHECK_PUREVIRTUALS
    #define PURE_VIRTUAL( InFunc, InExtra ) =0;
#else
    #define PURE_VIRTUAL( InFunc, InExtra ) { Sys_Errorf( TEXT( "Pure virtual not implemented (%s)" ), TEXT( #InFunc ) ); InExtra }
#endif

#endif //CORE_H
