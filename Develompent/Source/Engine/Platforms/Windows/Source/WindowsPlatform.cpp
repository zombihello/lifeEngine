#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

#include "Misc/Types.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Guid.h"
#include "Containers/String.h"
#include "Logger/LoggerMacros.h"
#include "EngineLoop.h"
#include "D3D11RHI.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"
#include "WindowsImGUI.h"

// ----
// Platform specific globals variables
// ----

FBaseLogger*         GLog			= new FWindowsLogger();
FBaseFileSystem*     GFileSystem	= new FWindowsFileSystem();
FBaseWindow*         GWindow		= new FWindowsWindow();
FBaseRHI*            GRHI			= new FD3D11RHI();
FEngineLoop*         GEngineLoop	= new FEngineLoop();
EPlatformType        GPlatform		= PLATFORM_Windows;

// ----
// Platform specific functions
// ----

/**
 * Get formatted string (for Unicode strings)
 */
int appGetVarArgs( tchar* InOutDest, uint32 InDestSize, uint32 InCount, const tchar*& InFormat, va_list InArgPtr )
{
    return vswprintf( InOutDest, InCount, InFormat, InArgPtr );
}

/**
 * Get formatted string (for ANSI strings)
 */
int appGetVarArgsAnsi( achar* InOutDest, uint32 InDestSize, uint32 InCount, const achar*& InFormat, va_list InArgPtr )
{
    return vsnprintf( InOutDest, InCount, InFormat, InArgPtr );
}

/**
 * Create process
 */
void* appCreateProc( const tchar* InPathToProcess, const tchar* InParams, bool InLaunchDetached, bool InLaunchHidden, bool InLaunchReallyHidden, int32 InPriorityModifier, uint64* OutProcessId /* = nullptr */ )
{
    LE_LOG( LT_Log, LC_Dev, TEXT( "CreateProc %s %s" ), InPathToProcess, InParams );

	std::wstring				commandLine = FString::Format( TEXT( "%s %s" ), InPathToProcess, InParams );

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

	if ( !CreateProcessW( NULL, ( LPWSTR )commandLine.c_str(), &attributes, &attributes, TRUE, ( DWORD )createFlags, NULL, ( LPCWSTR )GFileSystem->GetCurrentDirectory().c_str(), ( LPSTARTUPINFOW ) &startupInfo, &procInfo ) )
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

/**
 * Retrieves the termination status of the specified process
 */
bool appGetProcReturnCode( void* InProcHandle, int32* OutReturnCode )
{
    return GetExitCodeProcess( ( HANDLE )InProcHandle, ( DWORD* )OutReturnCode ) && *( ( DWORD* )OutReturnCode ) != STILL_ACTIVE;
}

FGuid appCreateGuid()
{
	FGuid	guid;
	HRESULT result = CoCreateGuid( ( GUID* ) &guid );
	check( result == S_OK );
	return guid;
}