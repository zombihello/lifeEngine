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

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <SDL.h>

#include "Core/Misc/Types.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/Guid.h"
#include "Core/Containers/String.h"
#include "Core/Containers/StringConv.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Platforms/Windows/WindowsLogger.h"
#include "Core/Platforms/Windows/WindowsFileSystem.h"
#include "Core/Platforms/Windows/WindowsWindow.h"
#include "Core/Platforms/Windows/WindowsStackWalker.h"
#include "Core/Platforms/Windows/WindowsGlobals.h"
#include "Launch/EngineLoop.h"
#include "RHI/D3D11/D3D11RHI.h"

// ----
// Platform specific globals variables
// ----

CBaseLogger*         g_Log			= new CWindowsLogger();
CBaseFileSystem*     g_FileSystem	= new CWindowsFileSystem();
CBaseWindow*         g_Window		= new CWindowsWindow();
CBaseRHI*            g_RHI			= new CD3D11RHI();
EPlatformType        g_Platform		= PLATFORM_Windows;

// ---
// Classes
// ---

/**
 * @ingroup WindowsPlatform
 * Stack walker for Windows
 */
class CStackWalker : public StackWalker
{
public:
	/**
	 * Constructor
	 */
	CStackWalker() : StackWalker()
	{}

	/**
	 * Get buffer
	 * @return Return call stack buffer
	 */
	FORCEINLINE const std::wstring& GetBuffer() const
	{
		return buffer;
	}

protected:
	virtual void OnOutput( LPCSTR InText ) override
	{
		buffer += ANSI_TO_TCHAR( InText );
	}

	virtual void OnSymInit( LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName )  override
	{}

	virtual void OnLoadModule( LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion ) override 
	{}

	virtual void OnDbgHelpErr( LPCSTR szFuncName, DWORD gle, DWORD64 addr ) override
	{}

private:
	std::wstring	buffer;		/**< Output buffer of call stack */
};

// ----
// Platform specific functions
// ----

/*
==================
Sys_GetVarArgs
==================
*/
int Sys_GetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr )
{
    return vswprintf( InOutDest, InCount, InFormat, InArgPtr );
}

/*
==================
Sys_GetVarArgsAnsi
==================
*/
int Sys_GetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr )
{
    return vsnprintf( InOutDest, InCount, InFormat, InArgPtr );
}

/*
==================
Sys_CreateProc
==================
*/
void* Sys_CreateProc( const tchar* InPathToProcess, const tchar* InParams, bool InLaunchDetached, bool InLaunchHidden, bool InLaunchReallyHidden, int32 InPriorityModifier, uint64* OutProcessId /* = nullptr */ )
{
    Logf( TEXT( "CreateProc %s %s\n" ), InPathToProcess, InParams );

	std::wstring				commandLine = CString::Format( TEXT( "%s %s" ), InPathToProcess, InParams );

	PROCESS_INFORMATION			procInfo;
	SECURITY_ATTRIBUTES			attributes;
	attributes.nLength					= sizeof( SECURITY_ATTRIBUTES );
	attributes.lpSecurityDescriptor		= nullptr;
	attributes.bInheritHandle			= true;

	uint64						createFlags = NORMAL_PRIORITY_CLASS;
	if ( InPriorityModifier < 0 )
	{
		if ( InPriorityModifier == -1 )
		{
			createFlags = BELOW_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = IDLE_PRIORITY_CLASS;
		}
	}
	else if ( InPriorityModifier > 0 )
	{
		if ( InPriorityModifier == 1 )
		{
			createFlags = ABOVE_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = HIGH_PRIORITY_CLASS;
		}
	}
	if ( InLaunchDetached )
	{
		createFlags |= DETACHED_PROCESS;
	}

	uint64			flags = 0;
	uint32			showWindowFlags = SW_HIDE;
	if ( InLaunchReallyHidden )
	{
		flags = STARTF_USESHOWWINDOW;

		showWindowFlags = SW_HIDE;			// Epic's code doesn't properly hide the window, so let's do that
		if ( InLaunchDetached )				// if really hiding the window, and running detached, create a new console
		{
			createFlags = CREATE_NEW_CONSOLE;
		}
	}
	else if ( InLaunchHidden )
	{
		flags = STARTF_USESHOWWINDOW;
		showWindowFlags = SW_SHOWMINNOACTIVE;
	}

	STARTUPINFO		startupInfo = 
	{ 
		sizeof( STARTUPINFO ),				NULL,						NULL,						NULL,
		( DWORD )CW_USEDEFAULT,				( DWORD )CW_USEDEFAULT,		( DWORD )CW_USEDEFAULT,		( DWORD )CW_USEDEFAULT,
		NULL,								NULL,						NULL,						( DWORD )flags,
		( WORD )showWindowFlags,			NULL,						NULL,						NULL,
		NULL,								NULL
	};

	if ( !CreateProcessW( NULL, ( LPWSTR )commandLine.c_str(), &attributes, &attributes, TRUE, ( DWORD )createFlags, NULL, ( LPCWSTR )g_FileSystem->GetCurrentDirectory().c_str(), ( LPSTARTUPINFOW ) &startupInfo, &procInfo ) )
	{
		if ( OutProcessId )
		{
			*OutProcessId = 0;
		}

		return nullptr;
	}

	if ( OutProcessId )
	{
		*OutProcessId = procInfo.dwProcessId;
	}

	return ( void* )procInfo.hProcess;
}

/*
==================
Sys_GetProcReturnCode
==================
*/
bool Sys_GetProcReturnCode( void* InProcHandle, int32* OutReturnCode )
{
    return GetExitCodeProcess( ( HANDLE )InProcHandle, ( DWORD* )OutReturnCode ) && *( ( DWORD* )OutReturnCode ) != STILL_ACTIVE;
}

/*
==================
Sys_ShowMessageBox
==================
*/
void Sys_ShowMessageBox( const tchar* InTitle, const tchar* InMessage, EMessageBox Intype )
{
	uint32		flags = 0;
	switch ( Intype )
	{
	case MB_Info:		flags = SDL_MESSAGEBOX_INFORMATION;	break;
	case MB_Warning:	flags = SDL_MESSAGEBOX_WARNING;		break;
	case MB_Error:		flags = SDL_MESSAGEBOX_ERROR;		break;
	}

	SDL_ShowSimpleMessageBox( flags, TCHAR_TO_ANSI( InTitle ), TCHAR_TO_ANSI( InMessage ), nullptr );
}

/*
==================
Sys_DumpCallStack
==================
*/
void Sys_DumpCallStack( std::wstring& OutCallStack )
{
	CStackWalker	stackWalker;
	stackWalker.ShowCallstack();
	OutCallStack = stackWalker.GetBuffer();
}

/*
==================
Sys_RequestExit
==================
*/
void Sys_RequestExit( bool InForce )
{
	if ( InForce )
	{
		// Force immediate exit
		// Dangerous because config code isn't flushed, global destructors aren't called, etc
		// Suppress abort message and MS reports
		_set_abort_behavior( 0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT );
		abort();
	}
	else
	{
		// Tell the platform specific code we want to exit cleanly from the main loop.
		PostQuitMessage( 0 );
		g_IsRequestingExit = true;
	}
}

/*
==================
Sys_CreateGuid
==================
*/
CGuid Sys_CreateGuid()
{
	CGuid	guid;
	HRESULT result = CoCreateGuid( ( GUID* ) &guid );
	Assert( result == S_OK );
	return guid;
}

/*
==================
Sys_ComputerName
==================
*/
std::wstring Sys_ComputerName()
{
	static tchar	result[256] = TEXT( "" );
	if ( !result[0] )
	{
		DWORD	size = ARRAY_COUNT( result );
		GetComputerNameW( result, &size );
	}
	return result;
}

/*
==================
Sys_UserName
==================
*/
std::wstring Sys_UserName()
{
	static tchar	result[256] = TEXT( "" );
	if ( !result[0] )
	{
		DWORD	size = ARRAY_COUNT( result );
		GetUserNameW( result, &size );
	}
	return result;
}