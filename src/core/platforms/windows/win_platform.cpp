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

#include "pch_core.h"
#include "core/core_private.h"
#include "core/platforms/windows/win_stackwalker.h"

/**
 * @ingroup core
 * @brief Stack walker for Windows
 */
class CWindowsStackWalker : public StackWalker
{
public:
	/**
	 * @brief Constructor
	 */
	CWindowsStackWalker() : StackWalker()
	{}

	/**
	 * @brief Get buffer
	 * @return Return call stack buffer
	 */
	FORCEINLINE const std::string& GetBuffer() const
	{
		return buffer;
	}

protected:
	virtual void OnOutput( LPCSTR text ) override
	{
		buffer += text;
	}

	virtual void OnSymInit( LPCSTR szSearchPath, DWORD symOptions, LPCSTR szUserName )  override
	{}

	virtual void OnLoadModule( LPCSTR img, LPCSTR mod, DWORD64 baseAddr, DWORD size, DWORD result, LPCSTR symType, LPCSTR pdbName, ULONGLONG fileVersion ) override
	{}

	virtual void OnDbgHelpErr( LPCSTR szFuncName, DWORD gle, DWORD64 addr ) override
	{}

private:
	std::string			buffer;		/**< Output buffer for call stack */
};

/*
==================
Sys_CreateProc
==================
*/
void* Sys_CreateProc( const achar* pPathToProcess, const achar* pParams, bool bLaunchDetached, bool bLaunchHidden, int32 priorityModifier, uint64* pProcessId /* = nullptr */ )
{
	std::string						commandLine = L_sprintf( "%s %s", pPathToProcess, pParams );
	PROCESS_INFORMATION				procInfo;
	SECURITY_ATTRIBUTES				attributes;
	attributes.nLength				= sizeof( SECURITY_ATTRIBUTES );
	attributes.lpSecurityDescriptor	= nullptr;
	attributes.bInheritHandle		= true;

	uint64						createFlags = NORMAL_PRIORITY_CLASS;
	if ( priorityModifier < 0 )
	{
		if ( priorityModifier == -1 )
		{
			createFlags = BELOW_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = IDLE_PRIORITY_CLASS;
		}
	}
	else if ( priorityModifier > 0 )
	{
		if ( priorityModifier == 1 )
		{
			createFlags = ABOVE_NORMAL_PRIORITY_CLASS;
		}
		else
		{
			createFlags = HIGH_PRIORITY_CLASS;
		}
	}
	
	if ( bLaunchDetached )
	{
		createFlags |= DETACHED_PROCESS;
	}

	uint64			flags = 0;
	uint32			showWindowFlags = SW_HIDE;
	if ( bLaunchHidden )
	{
		flags = STARTF_USESHOWWINDOW;
		showWindowFlags = SW_HIDE;
		if ( bLaunchDetached )				// If hiding the window, and running detached, create a new console
		{
			createFlags = CREATE_NEW_CONSOLE;
		}
	}

	STARTUPINFO		startupInfo = 
	{ 
		sizeof( STARTUPINFO ),				NULL,						NULL,						NULL,
		( DWORD )CW_USEDEFAULT,				( DWORD )CW_USEDEFAULT,		( DWORD )CW_USEDEFAULT,		( DWORD )CW_USEDEFAULT,
		NULL,								NULL,						NULL,						( DWORD )flags,
		( WORD )showWindowFlags,			NULL,						NULL,						NULL,
		NULL,								NULL
	};

	achar		path[MAX_PATH];
	GetCurrentDirectoryA( MAX_PATH, path );

	if ( !CreateProcessA( NULL, ( LPSTR )commandLine.c_str(), &attributes, &attributes, TRUE, ( DWORD )createFlags, NULL, ( LPCSTR )path, ( LPSTARTUPINFOA )&startupInfo, &procInfo ) )
	{
		if ( pProcessId )
		{
			*pProcessId = 0;
		}

		return nullptr;
	}

	if ( pProcessId )
	{
		*pProcessId = procInfo.dwProcessId;
	}
	return ( void* )procInfo.hProcess;
}

/*
==================
Sys_GetProcReturnCode
==================
*/
bool Sys_GetProcReturnCode( void* pProcHandle, int32* pReturnCode )
{
	return GetExitCodeProcess( ( HANDLE )pProcHandle, ( DWORD* )pReturnCode ) && *( ( DWORD* )pReturnCode ) != STILL_ACTIVE;
}

/*
==================
Sys_IsProcRunning
==================
*/
bool Sys_IsProcRunning( void* pProcHandle )
{
	DWORD	waitResult = WaitForSingleObject( ( HANDLE )pProcHandle, 0 );
	return waitResult != WAIT_TIMEOUT ? false : true;
}

/*
==================
Sys_WaitForProc
==================
*/
void Sys_WaitForProc( void* pProcHandle )
{
	WaitForSingleObject( ( HANDLE )pProcHandle, INFINITE );
}

/*
==================
Sys_TerminateProc
==================
*/
void Sys_TerminateProc( void* pProcHandle )
{
	TerminateProcess( ( HANDLE )pProcHandle, 0 );
}

/*
==================
Sys_ShowMessageBox
==================
*/
void Sys_ShowMessageBox( const achar* pTitle, const achar* pMessage, EMessageBox type )
{
	uint32	flags = MB_OK;
	switch ( type )
	{
	case MESSAGE_BOX_INFO:		flags |= MB_ICONINFORMATION;	break;
	case MESSAGE_BOX_WARNING:	flags |= MB_ICONWARNING;		break;
	case MESSAGE_BOX_ERROR:		flags |= MB_ICONERROR;			break;
	}

	MessageBoxA( nullptr, pMessage, pTitle, flags );
}

/*
==================
Sys_DLL_LoadModule
==================
*/
dllHandle_t Sys_DLL_LoadModule( const achar* pDLLName )
{
	return LoadLibraryExA( pDLLName, nullptr, LOAD_WITH_ALTERED_SEARCH_PATH );
}

/*
==================
Sys_DLL_UnloadModule
==================
*/
void Sys_DLL_UnloadModule( dllHandle_t pDLLHandle )
{
	if ( pDLLHandle )
	{
		FreeLibrary( pDLLHandle );
	}
}

/*
==================
Sys_DLL_GetProcAddress
==================
*/
void* Sys_DLL_GetProcAddress( dllHandle_t pDLLHandle, const achar* pFuncName )
{
	if ( pDLLHandle )
	{
		return GetProcAddress( pDLLHandle, pFuncName );
	}
	return nullptr;
}

/*
==================
Sys_RequestExit
==================
*/
void Sys_RequestExit( bool bForce )
{
	if ( bForce )
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
		g_bRequestingExit = true;
	}
}

/*
==================
Sys_GetComputerName
==================
*/
const achar* Sys_GetComputerName()
{
	static achar	result[256] = "";
	if ( !result[0] )
	{
		DWORD	size = ARRAYSIZE( result );
		GetComputerNameA( result, &size );
	}
	return result;
}

/*
==================
Sys_GetUserName
==================
*/
const achar* Sys_GetUserName()
{
	static achar	result[256] = "";
	if ( !result[0] )
	{
		DWORD	size = ARRAYSIZE( result );
		GetUserNameA( result, &size );
	}
	return result;
}

/*
==================
Sys_Seconds
==================
*/
double Sys_Seconds()
{
	LARGE_INTEGER		cycles;
	QueryPerformanceCounter( &cycles );

	// Add big number to make bugs apparent where return value is being passed to FLOAT
	return cycles.QuadPart * g_SecondsPerCycle + 16777216.0;
}

/*
==================
Sys_DumpCallStack
==================
*/
std::string Sys_DumpCallStack()
{
	CWindowsStackWalker		stackWalker;
	stackWalker.ShowCallstack();
	return stackWalker.GetBuffer();
}

/*
==================
Sys_GetSecondsPerCycle
==================
*/
double Sys_GetSecondsPerCycle()
{
	LARGE_INTEGER		frequency;
	bool				bResult = QueryPerformanceFrequency( &frequency );
	
	Assert( bResult );
	return 1.0 / frequency.QuadPart;
}

/*
==================
Sys_SetupConsoleIO
==================
*/
void Sys_SetupConsoleIO()
{
#if !RETAIL
	static bool		bConsoleInited = false;
	if ( !bConsoleInited )
	{
		AllocConsole();
		freopen( "conin$", "r", stdin );
		freopen( "conout$", "w", stdout );
		freopen( "conout$", "w", stderr );
		bConsoleInited = true;
	}
#endif // !RETAIL
}