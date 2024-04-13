#include "Core.h"
#include "Reflection/Class.h"
#include "Misc/Misc.h"
#include "Misc/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"
#include "System/Config.h"
#include "Scripts/ScriptEngine.h"
#include "System/Package.h"
#include "Misc/TableOfContents.h"
#include "Misc/CommandLine.h"

// ----------------
// GLOBALS
// ----------------

bool	                g_IsRequestingExit           = false;
bool                    g_IsRunning                  = false;
bool                    g_IsInitialLoad              = true;
uint32			        g_GameThreadId               = 0;
double                  g_SecondsPerCycle            = 0.0;
double                  g_StartTime                  = Sys_InitTiming();
double                  g_CurrentTime                = g_StartTime;
double                  g_LastTime                   = 0.0;
double                  g_DeltaTime                  = 0.0;
CPackageManager*        g_PackageManager             = new CPackageManager();
CTableOfContets		    g_TableOfContents;
std::wstring            g_GameName                   = TEXT( "ExampleGame" );
CCommandLine			g_CommandLine;
CAssetFactory           g_AssetFactory;

#if WITH_EDITOR
bool					g_IsGame                     = true;
bool			        g_IsEditor                   = false;
bool                    g_IsCommandlet               = false;
bool					g_IsCooker                   = false;
bool                    g_ShouldPauseBeforeExit      = false;
#endif // WITH_EDITOR

/*
==================
Sys_Error
==================
*/
void Sys_Error( const tchar* InFormat, ... )
{
	// Dump call stack
	std::wstring        callStack;
	Sys_DumpCallStack( callStack );

	// Get formated string
	va_list			params;
	va_start( params, InFormat );
	std::wstring	message = L_Vsprintf( InFormat, params );
	message += L_Sprintf( TEXT( "\n\nStack:\n%s" ), callStack.c_str() );
	va_end( params );

	// Print message and show message box
	Errorf( TEXT( "\n" ) );
	Errorf( TEXT( "--------------- FATAL ERROR ----------------\n" ) );
	Errorf( TEXT( "%s\n" ), message.c_str() );
	Errorf( TEXT( "--------------------------------------------\n" ) );
	Errorf( TEXT( "\n" ) );
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugBreak();
	}
	Sys_ShowMessageBox( TEXT( "LifeEngine Error" ), message.c_str(), MB_Error );

	// Shutdown application
	Sys_RequestExit( true );
}

#if ENABLED_ASSERT
/*
==================
Sys_FailAssertFunc
==================
*/
void Sys_FailAssertFunc( const tchar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
	// Dump call stack
	std::wstring        callStack;
	Sys_DumpCallStack( callStack );

	// Get final message
	va_list			params;
	va_start( params, InFormat );
	std::wstring	message = L_Sprintf( TEXT( "Assertion failed: %s\nMessage: %s\n\nFile: %s\nLine: %i\nStack:\n%s" ), InExpr, L_Strlen( InFormat ) > 0 ? L_Vsprintf( InFormat, params ).c_str() : TEXT( "<None>" ), ANSI_TO_TCHAR( InFile ), InLine, callStack.c_str() );
	va_end( params );

	// Print message and show message box
	Errorf( TEXT( "\n" ) );
	Errorf( TEXT( "------------ ASSERTION FAILED --------------\n" ) );
	Errorf( TEXT( "%s\n" ), message.c_str() );
	Errorf( TEXT( "--------------------------------------------\n" ) );
	Errorf( TEXT( "\n" ) );
	if ( Sys_IsDebuggerPresent() )
	{
		Sys_DebugBreak();
	}
	Sys_ShowMessageBox( TEXT( "LifeEngine Error" ), message.c_str(), MB_Error );

	// Shutdown application
	Sys_RequestExit( true );
}
#endif // ENABLED_ASSERT

/*
==================
Sys_PlatformTypeToString
==================
*/
std::wstring Sys_PlatformTypeToString( EPlatformType InPlatform )
{
    switch ( InPlatform )
    {
    case PLATFORM_Windows:      return TEXT( "PC" );
    default:                    return TEXT( "" );
    }
}

/*
==================
Sys_PlatformStringToType
==================
*/
EPlatformType Sys_PlatformStringToType( const std::wstring& InPlatformStr )
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