#include "Core.h"
#include "Misc/Class.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/Config.h"
#include "Scripts/ScriptEngine.h"

// ----------------
// GLOBALS
// ----------------

FConfig         GEngineConfig;
FConfig         GGameConfig;
FConfig         GInputConfig;
bool	        GIsRequestingExit           = false;
uint32			GGameThreadId               = 0;

#if WITH_EDITOR
FConfig         GEditorConfig;
bool			GIsEditor = false;
#endif // WITH_EDITOR

/**
 * Failed assertion handler
 */
void VARARGS appFailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    va_list     arguments;
    va_start( arguments, InFormat );

    tchar*      message = FString::Format( InFormat, arguments );
    LE_LOG( LT_Error, LC_None, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, message );

    va_end( arguments );
    free( message );
}

/**
 * Failed assertion handler debug version
 */
void VARARGS appFailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
	va_list     arguments;
	va_start( arguments, InFormat );

	tchar*      message = FString::Format( InFormat, arguments );
    LE_LOG( LT_Error, LC_None, TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack: Not avail yet" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, message );

	va_end( arguments );
	free( message );
}