#include "Core.h"
#include "Misc/Class.h"
#include "Misc/Misc.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/Config.h"
#include "Scripts/ScriptEngine.h"
#include "System/Package.h"
#include "Misc/TableOfContents.h"

// ----------------
// GLOBALS
// ----------------

CConfigManager          GConfig;
bool	                GIsRequestingExit           = false;
uint32			        GGameThreadId               = 0;
double                  GSecondsPerCycle            = 0.0;
double                  GStartTime                  = appInitTiming();
double                  GCurrentTime                = GStartTime;
double                  GLastTime                   = 0.0;
double                  GDeltaTime                  = 0.0;
CPackageManager*        GPackageManager             = new CPackageManager();
CTableOfContets		    GTableOfContents;
std::wstring            GGameName                   = TEXT( "ExampleGame" );

#if WITH_EDITOR
bool					GIsGame                     = true;
bool			        GIsEditor                   = false;
bool                    GIsCommandlet               = false;
bool					GIsCooker                   = false;
bool                    GShouldPauseBeforeExit      = false;
#endif // WITH_EDITOR

/**
 * Failed assertion handler
 */
void VARARGS appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
	std::wstring        callStack;
	appDumpCallStack( callStack );

	va_list             arguments;
	va_start( arguments, InFormat );
	std::wstring        message = CString::Format( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, CString::Format( InFormat, arguments ).c_str(), callStack.c_str() );
	va_end( arguments );

	LE_LOG( LT_Error, LC_General, message.c_str() );
	appShowMessageBox( CString::Format( TEXT( "%s Error" ), GGameName.c_str() ).c_str(), message.c_str(), MB_Error );
    appRequestExit( true );
}

/**
 * Failed assertion handler debug version
 */
void VARARGS appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    std::wstring        callStack;
    appDumpCallStack( callStack );

	va_list             arguments;
	va_start( arguments, InFormat );
	std::wstring        message = CString::Format( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, CString::Format( InFormat, arguments ).c_str(), callStack.c_str() );
	va_end( arguments );

	LE_LOG( LT_Error, LC_General, message.c_str() );
}

std::wstring appPlatformTypeToString( EPlatformType InPlatform )
{
    switch ( InPlatform )
    {
    case PLATFORM_Windows:      return TEXT( "PC" );
    default:                    return TEXT( "" );
    }
}

EPlatformType appPlatformStringToType( const std::wstring& InPlatformStr )
{
    if ( InPlatformStr == TEXT( "PC" ) )
    {
        return PLATFORM_Windows;
    }
    else
    {
        return PLATFORM_Unknown;
    }
}