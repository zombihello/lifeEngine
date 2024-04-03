#include "Core.h"
#include "System/Threading.h"
#include "WindowsThreading.h"
#include "Misc/StringConv.h"

/**
 * Code setting the thread name for use in the debugger.
 *
 * http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
 */
#define MS_VC_EXCEPTION				0x406D1388

#pragma pack( push, 8 )
typedef struct tagTHREADNAME_INFO
{
	DWORD		dwType;			// Must be 0x1000.
	LPCSTR		szName;			// Pointer to name (in user addr space).
	DWORD		dwThreadID;		// Thread ID (-1=caller thread).
	DWORD		dwFlags;		// Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack( pop )

/*
==================
SetThreadName
==================
*/
void SetThreadName( HANDLE InThreadHandle, const achar* InThreadName )
{
	THREADNAME_INFO			threadNameInfo;
	threadNameInfo.dwType		= 0x1000;
	threadNameInfo.szName		= InThreadName;
	threadNameInfo.dwThreadID	= ::GetThreadId( InThreadHandle );
	threadNameInfo.dwFlags		= 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( threadNameInfo ) / sizeof( ULONG_PTR ), ( ULONG_PTR* ) &threadNameInfo ) ;
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/*
==================
SetThreadPriority
==================
*/
void SetThreadPriority( HANDLE InThreadHandle, EThreadPriority InThreadPriority )
{
	Assert( InThreadPriority == TP_Normal || InThreadPriority == TP_Low || InThreadPriority == TP_AboveNormal || InThreadPriority == TP_BelowNormal || InThreadPriority == TP_High || InThreadPriority == TP_Realtime );
	SetThreadPriority( InThreadHandle,
		InThreadPriority == TP_Low ? THREAD_PRIORITY_LOWEST :
		InThreadPriority == TP_BelowNormal ? THREAD_PRIORITY_BELOW_NORMAL :
		InThreadPriority == TP_AboveNormal ? THREAD_PRIORITY_NORMAL :
		InThreadPriority == TP_High ? THREAD_PRIORITY_ABOVE_NORMAL :
		InThreadPriority == TP_Realtime ? THREAD_PRIORITY_HIGHEST :
		THREAD_PRIORITY_NORMAL );
}

/*
==================
CWindowsRunnableThread::CWindowsRunnableThread
==================
*/
CWindowsRunnableThread::CWindowsRunnableThread()
	: handle( nullptr )
	, threadInitSyncEvent( nullptr )
	, bAutoDeleteSelf( false )
	, bAutoDeleteRunnable( false )
{}

/*
==================
CWindowsRunnableThread::~CWindowsRunnableThread
==================
*/
CWindowsRunnableThread::~CWindowsRunnableThread()
{
	Kill( true );
}

/*
==================
CWindowsRunnableThread::SetPriority
==================
*/
void CWindowsRunnableThread::SetPriority( EThreadPriority InPriority )
{
	if ( IsAlive() )
	{
		SetThreadPriority( handle, InPriority );
		threadPriority = InPriority;
	}
}

/*
==================
CWindowsRunnableThread::Suspend
==================
*/
void CWindowsRunnableThread::Suspend( bool InIsShouldPause /* = true */ )
{
	// Do nothing if thread isn't alive
	if ( !IsAlive() )
	{
		return;
	}

	if ( InIsShouldPause )
	{
		SuspendThread( handle );
	}
	else
	{
		ResumeThread( handle );
	}
}

/*
==================
CWindowsRunnableThread::Kill
==================
*/
void CWindowsRunnableThread::Kill( bool InIsShouldWait /* = false */, int32 InExitCode /* = 0 */ )
{
	// Do nothing if the thread isn't alive
	if ( !handle )
	{
		return;
	}

	// Let the runnable have a chance to stop without brute force killing
	if ( runnable )
	{
		runnable->Stop();
	}

	// If waiting was specified, wait the amount of time. If that fails,
	// brute force kill that thread. Very bad as that might leak
	if ( InIsShouldWait )
	{
		// Wait indefinitely for the thread to finish
		// IMPORTANT: It's not safe to just go and kill the thread with TerminateThread() as 
		// it could have a mutex lock that's shared with a thread that's continuing to run, 
		// which would cause that other thread to dead-lock
		WaitForSingleObject( handle, INFINITE );
	}

	// Set exit code
	exitCode = InExitCode;

	// Now clean up the thread handle so we don't leak
	threadID = ( uint32 )-1;
	CloseHandle( handle );
	handle = nullptr;

	// delete the runnable if requested and we didn't shut down gracefully already.
	if ( runnable && bAutoDeleteRunnable )
	{
		delete runnable;
		runnable = nullptr;
	}

	// Delete ourselves if requested
	if ( bAutoDeleteSelf )
	{
		delete this;
	}
}

/*
==================
CWindowsRunnableThread::WaitForCompletion
==================
*/
void CWindowsRunnableThread::WaitForCompletion()
{
	// Block until this thread exits
	WaitForSingleObject( handle, INFINITE );
}

/*
==================
CWindowsRunnableThread::Create
==================
*/
bool CWindowsRunnableThread::CreateInternal( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf /* = false */, bool InIsAutoDeleteRunnable /* = false */, uint32 InStackSize /* = 0 */, EThreadPriority InThreadPriority /* = TP_Normal */ )
{
	// Remember our inputs
	runnable				= InRunnable;
	bAutoDeleteSelf			= InIsAutoDeleteSelf;
	bAutoDeleteRunnable		= InIsAutoDeleteRunnable;
	threadPriority			= InThreadPriority;

	// Create a sync event to guarantee the CRunnable::Init() function is called first
	threadInitSyncEvent = new CWindowsEvent();

	// Create a new thread
	handle = CreateThread( nullptr, InStackSize, &CWindowsRunnableThread::StaticMainProc, this, 0, ( LPDWORD ) &threadID );
	
	// If it fails, clear all the vars
	if ( !handle )
	{		
		if ( bAutoDeleteRunnable )
		{
			delete runnable;
		}

		runnable = nullptr;
	}
	else
	{
		// Let the thread start up, then set the name for debug purposes
		threadInitSyncEvent->Wait( INFINITE );
		SetThreadName( handle, InThreadName ? TCHAR_TO_ANSI( InThreadName ) : "Unnamed LE" );
	}

	// Cleanup the sync event
	delete threadInitSyncEvent;
	threadInitSyncEvent = nullptr;
	return !!handle;
}

/*
==================
CWindowsRunnableThread::StaticMainProc
==================
*/
DWORD CWindowsRunnableThread::StaticMainProc( LPVOID InThis )
{
	CWindowsRunnableThread*		thisThread = ( CWindowsRunnableThread* )InThis;
	return thisThread->Run();
}

/*
==================
CWindowsRunnableThread::Run
==================
*/
uint32 CWindowsRunnableThread::Run()
{
	Assert( runnable );
	SetThreadPriority( handle, threadPriority );

	// Initialize the runnable object
	bool		bInitReturn = runnable->Init();
	Assert( bInitReturn );
	UNUSED_VAR( bInitReturn );

	// Initialization has completed, release the sync event
	threadInitSyncEvent->Trigger();

	// Now run the task that needs to be done
	exitCode = runnable->Run();

	// Allow any allocated resources to be cleaned up
	runnable->Exit();

	// Should we delete the runnable?
	if ( bAutoDeleteRunnable )
	{
		delete runnable;
		runnable = nullptr;
	}

	// Clean ourselves up without waiting
	if ( bAutoDeleteSelf )
	{
		// Now clean up the thread handle so we don't leak
		CloseHandle( handle );
		handle		= nullptr;
		threadID	= ( uint32 )-1;
		delete this;
	}

	// Return from the thread with the exit code
	return exitCode;
}