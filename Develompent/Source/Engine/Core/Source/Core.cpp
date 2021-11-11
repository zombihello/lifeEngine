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

// ----------------
// GLOBALS
// ----------------

FConfig                 GEngineConfig;
FConfig                 GGameConfig;
FConfig                 GInputConfig;
bool	                GIsRequestingExit           = false;
uint32			        GGameThreadId               = 0;
double                  GSecondsPerCycle            = 0.0;
double                  GStartTime                  = appInitTiming();
double                  GCurrentTime                = GStartTime;
double                  GLastTime                   = 0.0;
double                  GDeltaTime                  = 0.0;
FPackageManager*        GPackageManager             = new FPackageManager();

#if WITH_EDITOR
FConfig                 GEditorConfig;
bool			        GIsEditor                   = false;
bool                    GIsCommandlet               = false;
#endif // WITH_EDITOR

/**
 * Failed assertion handler
 */
void VARARGS appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    va_list     arguments;
    va_start( arguments, InFormat );
    LE_LOG( LT_Error, LC_None, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, FString::Format( InFormat, arguments ).c_str() );
    va_end( arguments );
}

/**
 * Failed assertion handler debug version
 */
void VARARGS appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
	va_list     arguments;
	va_start( arguments, InFormat );
    LE_LOG( LT_Error, LC_None, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, FString::Format( InFormat, arguments ).c_str() );
	va_end( arguments );
}