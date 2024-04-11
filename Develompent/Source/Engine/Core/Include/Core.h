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
#include "System/Memory.h"
#include "Misc/Template.h"
#include "Misc/Types.h"
#include "Misc/Names.h"
#include "Misc/Platform.h"
#include "Misc/CoreGlobals.h"

/**
 * @ingroup Core
 * @brief Enumeration for no init structs and classes
 */
enum ENoInit
{
    NoInit
};

/**
 * @ingroup Core
 */
enum
{
    INVALID_HASH    = ( uint64 )-1,    /**< Invalid hash */
    INVALID_ID      = ( uint32 )-1,    /**< Invalid ID */
    INDEX_NONE      = ( uint32 )-1     /**< Invalid index */
};

/**
 * @ingroup Core
 * @brief Platform type
 */
enum EPlatformType
{
    PLATFORM_Unknown,           /**< Unknown platform */
    PLATFORM_Windows            /**< Windows */
};

/**
 * @ingroup Core
 * @brief Message box type
 */
enum EMessageBox
{
    MB_Info,        /**< Info */
    MB_Warning,     /**< Warning */
    MB_Error        /**< Error */
};

/**
 * @ingroup Core
 * @brief Convert EPlatformType to string
 * 
 * @param InPlatform    The platform of interest
 * @return Return the string name of the given platform. If the platform is unknown returns empty string
 */
std::wstring Sys_PlatformTypeToString( EPlatformType InPlatform );

/**
 * @ingroup Core
 * @brief Convert string to EPlatformType
 * 
 * @param InPlatformStr     The platform of interest
 * @return Return the enumeration value for the given platform. If the platform is unknown returns PLATFORM_Unknown
 */
EPlatformType Sys_PlatformStringToType( const std::wstring& InPlatformStr );

/**
 * @ingroup Core
 * @brief Macro for convert string to WChar
 *
 * @param String    Input string
 *
 * Example usage: @code TEXT( "Hello World" ) @endcode
 */
#undef TEXT
#define TEXT( String )          L##String

/**
 * @ingroup Core
 * @brief Macro for empty code
 */
#define NOOP                    ( void )0

/**
 * @ingroup Core
 * @brief Macro for labeling a variable that it is not used
 * 
 * @param Value     Value
 * 
 * Example usage: @code UNUSED_VAR( health ); @endcode
 */
#define UNUSED_VAR( Value )     ( void )Value

/**
 * @ingroup Core
 * @brief Macro for mark deprecated code
 *
 * @param Version   Engine version where this code will be removed
 * @param Message   Custom message
 *
 * Example usage:
 * @code
 * DEPRECATED( 1.0.0, "Your message" )
 * void Function()
 * {}
 * @endcode
 */
#define DEPRECATED( Version, Message )			        [ [ deprecated( Message " Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile." ) ] ]

/**
 * @ingroup Core
 * @brief Pre-Initialize platform
 * @note Need implement on each platform
 * @return Return the error level, 0 - successful and more than 0 - have been errors
 */
int32 Sys_PlatformPreInit();

/**
 * @ingroup Core
 * @brief Initialize platform
 * @note Need implement on each platform
 * @return Returns the error level, 0 - successful and more than 0 - have been errors 
 */
int32 Sys_PlatformInit();

/**
 * @ingroup Core
 * @brief Process window events
 * @note Need implement on each platform
 */
void Sys_ProcessWindowEvents();

/**
 * @ingroup Core
 * @brief Create process
 * @note Need implement on each platform
 * 
 * @param InPathToProcess       Executable name
 * @param InParams              Command line arguments
 * @param InLaunchDetached      If TRUE, the new process will have its own window
 * @param InLaunchHidden        If TRUE, the new process will not have a window or be in the task bar
 * @param InPriorityModifier    -2 idle, -1 low, 0 normal, 1 high, 2 higher
 * @param OutProcessId          If non-NULL, this will be filled in with the ProcessId
 * @return Return the process handle for use in other process functions
 */
void* Sys_CreateProc( const tchar* InPathToProcess, const tchar* InParams, bool InLaunchDetached, bool InLaunchHidden, int32 InPriorityModifier, uint64* OutProcessId = nullptr );

/**
 * @ingroup Core
 * @brief Retrieves the termination status of the specified process
 * @note Need implement on each platform
 * 
 * @param InProcHandle      Handle of process
 * @param OutReturnCode     Return code
 * @return Return TRUE if process is end, otherwise returns FALSE
 */
bool Sys_GetProcReturnCode( void* InProcHandle, int32* OutReturnCode );

/**
 * @ingroup Core
 * @brief Is process running
 * @note Need implement on each platform
 * 
 * @param InProcHandle   Handle of process returned from Sys_CreateProc
 * @return Return TRUE if process is running, otherwise returns FALSE
 */
bool Sys_IsProcRunning( void* InProcHandle );

/**
 * @ingroup Core
 * @brief Waits for a process to stop
 * @note Need implement on each platform
 * 
 * @param InProcHandle   Handle of process returned from Sys_CreateProc
 */
void Sys_WaitForProc( void* InProcHandle );

/**
 * @ingroup Core
 * @brief Terminates a process
 * @note Need implement on each platform
 * 
 * @param InProcHandle   Handle of process returned from Sys_CreateProc
 */
void Sys_TerminateProc( void* InProcHandle );

/**
 * @ingroup Core
 * @brief Show message box
 * @note Need implement on each platform
 * 
 * @param InTitle   Title
 * @param InMessage Message
 * @param InType    Message type
 */
void Sys_ShowMessageBox( const tchar* InTitle, const tchar* InMessage, EMessageBox Intype );

/**
 * @ingroup Core
 * @brief Dump call stack
 * @note Need implement on each platform
 * @param OutCallStack  Output call stack
 */
void Sys_DumpCallStack( std::wstring& OutCallStack );

/**
 * @ingroup Core
 * @brief Request shutdown application
 * @note Need implement on each platform
 * @param InForce   Is need force shutdown application
 */
void Sys_RequestExit( bool InForce );

/**
 * @ingroup Core
 * @brief Get computer name
 * @note Need implement on each platform
 * @return Return computer name
 */
std::wstring Sys_GetComputerName();

/**
 * @ingroup Core
 * @brief Get user name
 * @note Need implement on each platform
 * @return Return user name
 */
std::wstring Sys_GetUserName();

/**
 * @ingroup Core
 * @brief Does per platform initialization of timing information and returns the current time
 * @note Need implement on each platform, internal usage only!
 * @return Return current time
 */
double Sys_InitTiming();

/**
 * @ingroup Core
 * @brief Get time in seconds. Origin is arbitrary
 * @note Need implement on each platform
 * @reutrn Return time in seconds
 */
double Sys_Seconds();

/**
 * @ingroup Core
 * @brief Print critical error and shutdown application
 *
 * @param InFormat	Display message format
 * @param ...		Message parameters
 *
 * Example usage: @code Sys_Error( TEXT( "Hello Error %i" ), 43 ); @endcode
 */
void Sys_Error( const tchar* InFormat, ... );

/**
 * @ingroup Core
 * @brief Set clipboard text
 * @note Need implement on each platform
 * @param InText    The text to clipboard
 */
void Sys_SetClipboardText( const std::wstring& InText );

/**
 * @ingroup Core
 * @brief Get clipboard text
 * @note Need implement on each platform
 * @return Return text from clipboard
 */
std::wstring Sys_GetClipboardText();

#if ENABLED_ASSERT || PLATFORM_DOXYGEN
    /**
     * @ingroup Core
     * @brief Failed assertion handler
     *
     * @param InExpr   Condition that was not met
     * @param InFile   File path with error
     * @param InLine   Error line
     * @param InFormat Displayed message format
     * @param ...      Message parameters
     *
     * Example usage: @code Sys_FailAssertFunc( TEXT( "Value != 0" ), __FILE__, __LINE__, TEXT( "Value = %i" ), Value ); @endcode
     */
    void Sys_FailAssertFunc( const tchar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... );

    /**
    * @ingroup Core
    * @brief Macro for checking the condition with your code
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param Code   Verification code
    *
    * Example usage: @code AssertCode( if( !isValid ) Sys_Error( TEXT( "Value not valid" ) ) ) @endcode
    */
    #define AssertCode( Code ) \
        do \
        { \
            Code \
        } \
        while ( false );

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, display your message
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param Expr   Checkable condition
    * @param Msg    Message
    *
    * Example usage: @code AssertMsg( Value == 0, TEXT( "Value = %i" ), Value ) @endcode
    */
    #define AssertMsg( Expr, Msg, ... ) \
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( TEXT( #Expr ), __FILE__, __LINE__, Msg, __VA_ARGS__ ); \
        } \
    }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition and in case of failure, call your function
    * @warning With enabled define ENABLED_ASSERT this macro is empty
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
            Sys_FailAssertFunc( TEXT( #Expr ), __FILE__, __LINE__ ); \
        } \
    }

    /**
    * @ingroup Core
    * @brief Macro for checking the condition
    * @warning With enabled define ENABLED_ASSERT this macro is empty
    *
    * @param Expr   Checkable condition
    *
    * Example usage: @code Assert( Value == 0 ) @endcode
    */
    #define Assert( Expr ) \
    { \
        if ( !( Expr ) ) \
        { \
            Sys_FailAssertFunc( TEXT( #Expr ), __FILE__, __LINE__ ); \
        } \
    }

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
    #define AssertNoEntry()                     Sys_FailAssertFunc( TEXT( "Enclosing block should never be called" ), __FILE__, __LINE__ );

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
#define AssertNoReentry() \
    { \
        static bool s_bBeenHere##__LINE__ = false; \
        AssertMsg( !s_bBeenHere##__LINE__, TEXT( "Enclosing block was called more than once" ) ); \
        s_bBeenHere##__LINE__ = true; \
    }
#else
    FORCEINLINE void Sys_FailAssertFunc( const tchar* InExpr, const achar* InFile, int InLine, const tchar* InFormat = TEXT( "" ), ... ) {}
    #define AssertCode( Code )		        {}
    #define AssertMsg( Expr, Msg, ... )		{}
    #define AssertFunc( Expr, Func )	    {}
    #define Assert( Expr )				    {}
    #define AssertNoEntry()                 {}
    #define AssertNoReentry()               {}
#endif // ENABLED_ASSERT

// This define must come after the class declaration for CBaseLogger, since the macro uses g_Log->Logf
#if CHECK_PUREVIRTUALS
    #define PURE_VIRTUAL( InFunc, InExtra ) =0;
#else
    #define PURE_VIRTUAL( InFunc, InExtra ) { Sys_Error( TEXT( "Pure virtual not implemented (%s)" ), TEXT( #InFunc ) ); InExtra }
#endif

#endif //CORE_H
