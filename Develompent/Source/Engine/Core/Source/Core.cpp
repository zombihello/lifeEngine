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

CConfigManager          g_Config;
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
Sys_FailAssertFunc
==================
*/
void VARARGS Sys_FailAssertFunc( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
	std::wstring        callStack;
	Sys_DumpCallStack( callStack );

	va_list             arguments;
	va_start( arguments, InFormat );
	std::wstring        message = L_Sprintf( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, L_Vsprintf( InFormat, arguments ).c_str(), callStack.c_str() );
	va_end( arguments );

	Errorf( TEXT( "%s\n" ), message.c_str());
	Sys_ShowMessageBox( L_Sprintf( TEXT( "%s Error" ), g_GameName.c_str() ).c_str(), message.c_str(), MB_Error );
    Sys_RequestExit( true );
}

/*
==================
Sys_FailAssertFuncDebug
==================
*/
void VARARGS Sys_FailAssertFuncDebug( const achar* InExpr, const achar* InFile, int InLine, const tchar* InFormat, ... )
{
    std::wstring        callStack;
    Sys_DumpCallStack( callStack );

	va_list             arguments;
	va_start( arguments, InFormat );
	std::wstring        message = L_Sprintf( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, L_Vsprintf( InFormat, arguments ).c_str(), callStack.c_str() );
	va_end( arguments );

	Errorf( TEXT( "%s\n" ), message.c_str());
}

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