/**
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

#include "Core/Core.h"
#include "Core/Misc/Class.h"
#include "Core/Misc/Misc.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Logger/BaseLogger.h"
#include "Core/System/Config.h"
#include "Core/System/Package.h"
#include "Core/Misc/TableOfContents.h"
#include "Core/Misc/CommandLine.h"
#include "Engine/Scripts/ScriptEngine.h"

// ----------------
// GLOBALS
// ----------------

CConfigManager          g_Config;
bool	                g_IsRequestingExit           = false;
uint32			        g_GameThreadId               = 0;
double                  g_SecondsPerCycle            = 0.0;
double                  g_StartTime                  = Sys_InitTiming();
double                  g_CurrentTime                = g_StartTime;
double                  g_LastTime                   = 0.0;
double                  g_DeltaTime                  = 0.0;
CPackageManager*        g_PackageManager             = new CPackageManager();
CTableOfContets		    g_TableOfContents;
CCommandLine			g_CommandLine;
CAssetFactory           g_AssetFactory;

#if WITH_EDITOR
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
	std::wstring        message = CString::Format( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, CString::Format( InFormat, arguments ).c_str(), callStack.c_str() );
	va_end( arguments );

	Errorf( TEXT( "%s\n" ), message.c_str());
	Sys_ShowMessageBox( CString::Format( TEXT( "%s Error" ), CApplication::Get().GetName().c_str() ).c_str(), message.c_str(), MB_Error );
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
	std::wstring        message = CString::Format( TEXT( "Assertion failed: %s [File:%s] [Line: %i]\n%s\nStack:\n%s" ), ANSI_TO_TCHAR( InExpr ), ANSI_TO_TCHAR( InFile ), InLine, CString::Format( InFormat, arguments ).c_str(), callStack.c_str() );
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