#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>
#include <SDL.h>

#include "Misc/Types.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Guid.h"
#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "EngineLoop.h"
#include "D3D11RHI.h"
#include "WindowsLogger.h"
#include "WindowsFileSystem.h"
#include "WindowsWindow.h"
#include "WindowsImGUI.h"
#include "WindowsStackWalker.h"

// ----
// Platform specific globals variables
// ----

CBaseLogger*         GLog			= new CWindowsLogger();
CBaseFileSystem*     GFileSystem	= new CWindowsFileSystem();
CBaseWindow*         GWindow		= new CWindowsWindow();
CBaseRHI*            GRHI			= new CD3D11RHI();
CEngineLoop*         GEngineLoop	= new CEngineLoop();
EPlatformType        GPlatform		= PLATFORM_Windows;

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

void appShowMessageBox( const tchar* InTitle, const tchar* InMessage, EMessageBox Intype )
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

void appDumpCallStack( std::wstring& OutCallStack )
{
	CStackWalker	stackWalker;
	stackWalker.ShowCallstack();
	OutCallStack = stackWalker.GetBuffer();
}

void appRequestExit( bool InForce )
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
		GIsRequestingExit = true;
	}
}

CGuid appCreateGuid()
{
	CGuid	guid;
	HRESULT result = CoCreateGuid( ( GUID* ) &guid );
	check( result == S_OK );
	return guid;
}