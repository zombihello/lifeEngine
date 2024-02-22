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

/**
* Code setting the thread name for use in the debugger
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
Sys_GetCurrentThreadHandle
==================
*/
threadHandle_t Sys_GetCurrentThreadHandle()
{
	return GetCurrentThread();
}

/*
==================
Sys_SetThreadPriority
==================
*/
void Sys_SetThreadPriority( threadHandle_t threadHandle, EThreadPriority threadPriority )
{
	Assert( threadPriority == THREAD_PRIOR_NORMAL || threadPriority == THREAD_PRIOR_LOW || threadPriority == THREAD_PRIOR_ABOVE_NORMAL || threadPriority == THREAD_PRIOR_BELOW_NORMAL || threadPriority == THREAD_PRIOR_HIGH || threadPriority == THREAD_PRIOR_REALTIME );
	SetThreadPriority( threadHandle,
		threadPriority == THREAD_PRIOR_LOW ? THREAD_PRIORITY_LOWEST :
		threadPriority == THREAD_PRIOR_BELOW_NORMAL ? THREAD_PRIORITY_BELOW_NORMAL :
		threadPriority == THREAD_PRIOR_ABOVE_NORMAL ? THREAD_PRIORITY_NORMAL :
		threadPriority == THREAD_PRIOR_HIGH ? THREAD_PRIORITY_ABOVE_NORMAL :
		threadPriority == THREAD_PRIOR_REALTIME ? THREAD_PRIORITY_HIGHEST :
		THREAD_PRIORITY_NORMAL );
}

/*
==================
Sys_SetThreadName
==================
*/
void Sys_SetThreadName( threadHandle_t threadHandle, const achar* pThreadName )
{
	THREADNAME_INFO				threadNameInfo;
	threadNameInfo.dwType		= 0x1000;
	threadNameInfo.szName		= pThreadName;
	threadNameInfo.dwThreadID	= GetThreadId( threadHandle );
	threadNameInfo.dwFlags		= 0;

	__try
	{
		RaiseException( MS_VC_EXCEPTION, 0, sizeof( threadNameInfo ) / sizeof( ULONG_PTR ), ( ULONG_PTR* )&threadNameInfo );
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

/*
==================
CWindowsThreadMutex::CWindowsThreadMutex
==================
*/
CWindowsThreadMutex::CWindowsThreadMutex()
{
	/**
	 * Constructor that initializes the aggregated critical section
	 * MSDN: You can improve performance significantly by choosing a small spin count for a critical section
	 * of short duration. The heap manager uses a spin count of roughly 4000 for its per-heap critical sections.
	 * This gives great performance and scalability in almost all worst-case scenarios
	 */
	const int32 spinCount = 4000;

	InitializeCriticalSection( &criticalSection );
	SetCriticalSectionSpinCount( &criticalSection, spinCount );
}

/*
==================
CWindowsThreadMutex::~CWindowsThreadMutex
==================
*/
CWindowsThreadMutex::~CWindowsThreadMutex()
{
	DeleteCriticalSection( &criticalSection );
}

/*
==================
CWindowsThreadMutex::Lock
==================
*/
void CWindowsThreadMutex::Lock()
{
	// Spin first before entering critical section, causing ring-0 transition and context switch.
	if ( TryEnterCriticalSection( &criticalSection ) == 0 )
	{
		EnterCriticalSection( &criticalSection );
	}
}

/*
==================
CWindowsThreadMutex::Lock
==================
*/
void CWindowsThreadMutex::Lock() const
{
	const_cast<CWindowsThreadMutex*>( this )->Lock();
}

/*
==================
CWindowsThreadMutex::Unlock
==================
*/
void CWindowsThreadMutex::Unlock()
{
	LeaveCriticalSection( &criticalSection );
}

/*
==================
CWindowsThreadMutex::Unlock
==================
*/
void CWindowsThreadMutex::Unlock() const
{
	const_cast<CWindowsThreadMutex*>( this )->Unlock();
}


/*
==================
CWindowsThreadEvent::CWindowsThreadEvent
==================
*/
CWindowsThreadEvent::CWindowsThreadEvent( bool bManualReset /* = false */, const achar* pName /* = nullptr */ )
	: handle( nullptr )
{
	handle = CreateEventA( nullptr, bManualReset, 0, pName );
	AssertMsg( handle, "Failed to create event (GetLastError 0x%X)", GetLastError() );
}

/*
==================
CWindowsThreadEvent::~CWindowsThreadEvent
==================
*/
CWindowsThreadEvent::~CWindowsThreadEvent()
{
	CloseHandle( handle );
}

/*
==================
CWindowsThreadEvent::Trigger
==================
*/
void CWindowsThreadEvent::Trigger()
{
	SetEvent( handle );
}

/*
==================
CWindowsThreadEvent::Reset
==================
*/
void CWindowsThreadEvent::Reset()
{
	ResetEvent( handle );
}

/*
==================
CWindowsThreadEvent::Pulse
==================
*/
void CWindowsThreadEvent::Pulse()
{
	PulseEvent( handle );
}

/*
==================
CWindowsThreadEvent::Wait
==================
*/
bool CWindowsThreadEvent::Wait( uint32 waitTime /* = -1 */ )
{
	return WaitForSingleObject( handle, waitTime ) == WAIT_OBJECT_0;
}


/*
==================
CWindowsThreadSemaphore::CWindowsThreadSemaphore
==================
*/
CWindowsThreadSemaphore::CWindowsThreadSemaphore( uint32 initialValue, uint32 maxValue, const achar* pName /* = nullptr */ )
	: handle( nullptr )
{
	AssertMsg( maxValue > 0, "Invalid max value for semaphore" );
	AssertMsg( initialValue >= 0 && initialValue <= maxValue, "Invalid initial value for semaphore" );
	handle = CreateSemaphoreA( nullptr, initialValue, maxValue, pName );
	AssertMsg( handle, "Failed to create semaphore (GetLastError 0x%X)", GetLastError() );
}

/*
==================
CWindowsThreadSemaphore::~CWindowsThreadSemaphore
==================
*/
CWindowsThreadSemaphore::~CWindowsThreadSemaphore()
{
	CloseHandle( handle );
}

/*
==================
CWindowsThreadSemaphore::Signal
==================
*/
bool CWindowsThreadSemaphore::Signal()
{
	return Post( 1 );
}

/*
==================
CWindowsThreadSemaphore::Post
==================
*/
bool CWindowsThreadSemaphore::Post( uint32 value )
{
	bool	bResult = ReleaseSemaphore( handle, value, nullptr );
	AssertMsg( bResult, "Failed to post semaphore (GetLastError 0x%X)", GetLastError() );
	return bResult;
}

/*
==================
CWindowsThreadSemaphore::Wait
==================
*/
void CWindowsThreadSemaphore::Wait()
{
	uint32	result = WaitForSingleObject( handle, INFINITE );
	Assert( result == WAIT_OBJECT_0 );
}

/*
==================
CWindowsThreadSemaphore::Wait
==================
*/
bool CWindowsThreadSemaphore::Wait( uint32 milliseconds )
{
	uint32	result = WaitForSingleObject( handle, milliseconds );
	Assert( result != WAIT_FAILED );
	return result != WAIT_TIMEOUT;
}

/*
==================
CWindowsThreadSemaphore::TryWait
==================
*/
bool CWindowsThreadSemaphore::TryWait()
{
	return Wait( 0 );
}


/*
==================
CWindowsThread::CWindowsThread
==================
*/
CWindowsThread::CWindowsThread()
	: exitCode( -1 )
	, threadPriority( THREAD_PRIOR_NORMAL )
	, handle( nullptr )
	, pThreadInitSyncEvent( nullptr )
{
	name[0] = '\0';
}

/*
==================
CWindowsThread::~CWindowsThread
==================
*/
CWindowsThread::~CWindowsThread()
{
	// Stop the thread if it is alive
	if ( IsAlive() )
	{
		Stop( true );
	}
}

/*
==================
CWindowsThread::Start
==================
*/
bool CWindowsThread::Start( uint32 stackSize /* = 0 */ )
{
	// We can't to start a thread who already started
	if ( IsAlive() )
	{
		AssertMsg( false, "Tried to create a thread that has already been created!" );
		return false;
	}

	// Create a sync event to guarantee Init() function is called first
	pThreadInitSyncEvent = new CWindowsThreadEvent( true );

	// Create a new thread
	handle = CreateThread( nullptr, stackSize, &CWindowsThread::ThreadMain, this, 0, nullptr );
	if ( !handle )
	{
		AssertMsg( false, "Failed to create thread (GetLastError 0x%x)", GetLastError() );
		return false;
	}

	// Let the thread start up
	pThreadInitSyncEvent->Wait( INFINITE );

	// Cleanup the sync event
	delete pThreadInitSyncEvent;
	pThreadInitSyncEvent = nullptr;
	return true;
}

/*
==================
CWindowsThread::Stop
==================
*/
void CWindowsThread::Stop( bool bShouldWait /* = false */, int32 exitCode /* = 0 */ )
{
	// Do nothing if the thread isn't alive
	if ( !IsAlive() )
	{
		return;
	}

	// Let the thread have a chance to stop without brute force killing
	CWindowsThread::exitCode = exitCode;
	ThreadStop();

	// If waiting was specified, wait the amount of time. If that fails,
	// brute force kill that thread. Very bad as that might leak
	if ( bShouldWait )
	{
		// Wait indefinitely for the thread to finish
		// 
		// IMPORTANT: It's not safe to just go and kill the thread with TerminateThread()
		// as it could have a mutex lock that's shared with a thread that's continuing to run, 
		// which would cause that other thread to dead-lock
		WaitForSingleObject( handle, INFINITE );
	}

	// Now clean up the thread handle so we don't leak
	CloseHandle( handle );
	handle		= nullptr;
	name[0]		= '\0';
}

/*
==================
CWindowsThread::Suspend
==================
*/
void CWindowsThread::Suspend( bool bShouldPause /* = true */ )
{
	// Do nothing if the thread isn't alive
	if ( !IsAlive() )
	{
		return;
	}

	// Suspend the thread if it need
	if ( bShouldPause )
	{
		SuspendThread( handle );
	}
	// Otherwise resume the one
	else
	{
		ResumeThread( handle );
	}
}

/*
==================
CWindowsThread::WaitForCompletion
==================
*/
void CWindowsThread::WaitForCompletion()
{
	if ( IsAlive() )
	{
		// Block until this thread exits
		WaitForSingleObject( handle, INFINITE );
	}
}

/*
==================
CWindowsThread::IsAlive
==================
*/
bool CWindowsThread::IsAlive() const
{
	return !!handle;
}

/*
==================
CWindowsThread::SetName
==================
*/
void CWindowsThread::SetName( const achar* pName )
{
	L_Strncpy( name, pName, sizeof( name ) - 1 );
	name[sizeof( name ) - 1] = '\0';
}

/*
==================
CWindowsThread::GetName
==================
*/
const achar* CWindowsThread::GetName() const
{
	if ( !name[0] )
	{
		achar*	pName = const_cast<achar*>( name );
		L_Snprintf( pName, sizeof( name ) - 1, "Thread(%p/%p)", this, handle );
		pName[sizeof( name ) - 1] = '\0';
	}
	return name;
}

/*
==================
CWindowsThread::SetPriority
==================
*/
void CWindowsThread::SetPriority( EThreadPriority priority )
{
	threadPriority = priority;
	if ( IsAlive() )
	{
		Sys_SetThreadPriority( handle, priority );
	}
}

/*
==================
CWindowsThread::GetPriority
==================
*/
EThreadPriority CWindowsThread::GetPriority() const
{
	return threadPriority;
}

/*
==================
CWindowsThread::GetThreadHandle
==================
*/
threadHandle_t CWindowsThread::GetThreadHandle() const
{
	return handle;
}

/*
==================
CWindowsThread::GetExitCode
==================
*/
int32 CWindowsThread::GetExitCode() const
{
	return exitCode;
}

/*
==================
CWindowsThread::ThreadInit
==================
*/
bool CWindowsThread::ThreadInit()
{
	return true;
}

/*
==================
CWindowsThread::ThreadStop
==================
*/
void CWindowsThread::ThreadStop()
{}

/*
==================
CWindowsThread::ThreadExit
==================
*/
void CWindowsThread::ThreadExit()
{}

/*
==================
CWindowsThread::ThreadMain
==================
*/
DWORD STDCALL CWindowsThread::ThreadMain( LPVOID pThis )
{
	CWindowsThread*		theThread = ( CWindowsThread* )pThis;

	// Set thread priority and debug name
	Sys_SetThreadPriority( theThread->handle, theThread->threadPriority );
	Sys_SetThreadName( theThread->handle, theThread->GetName() );

	// Initialize the thread
	theThread->exitCode = -1;
	bool	bInitResult = theThread->ThreadInit();

	// Initialization has completed, release the sync event
	theThread->pThreadInitSyncEvent->Trigger();

	// If the thread has not been initialized, close the thread
	if ( !bInitResult )
	{
		Warning( "Thread '%s' failed to initialize", theThread->GetName() );
		return theThread->exitCode;
	}

	// Now run the task that needs to be done
	try
	{
		theThread->exitCode = theThread->ThreadRun();
	}
	catch ( ... )
	{ }

	// Allow any allocated resources to be cleaned up
	theThread->ThreadExit();

	// Clean ourselves up without waiting
	// Now clean up the thread handle so we don't leak
	CloseHandle( theThread->handle );
	theThread->handle		= nullptr;
	theThread->name[0]		= '\0';

	// Return from the thread with the exit code
	return theThread->exitCode;
}