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

#include "Core/Containers/StringConv.h"
#include "Core/System/Threading.h"

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
SetThreadName
==================
*/
void SetThreadName( HANDLE InThreadHandle, const achar* InThreadName )
{
	THREADNAME_INFO				threadNameInfo;
	threadNameInfo.dwType		= 0x1000;
	threadNameInfo.szName		= InThreadName;
	threadNameInfo.dwThreadID	= ::GetThreadId( InThreadHandle );
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
CWindowsThread::CWindowsThread
==================
*/
CWindowsThread::CWindowsThread()
	: threadId( 0 )
	, handle( nullptr )
	, runnable( nullptr )
	, threadInitSyncEvent( nullptr )
	, threadPriority( TP_Normal )
	, bAutoDeleteSelf( false )
	, bAutoDeleteRunnable( false )
{}

/*
==================
CWindowsThread::CWindowsThread
==================
*/
CWindowsThread::~CWindowsThread()
{
	if ( handle )
	{
		Kill( true );
	}
}

/*
==================
CWindowsThread::Create
==================
*/
bool CWindowsThread::Create( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf /* = false */, bool InIsAutoDeleteRunnable /* = false */, uint32 InStackSize /* = 0 */, EThreadPriority InThreadPriority /* = TP_Normal */ )
{
	// Kill old thread if it was
	Kill( true );

	// Remember our inputs
	runnable				= InRunnable;
	bAutoDeleteSelf			= InIsAutoDeleteSelf;
	bAutoDeleteRunnable		= InIsAutoDeleteRunnable;
	threadPriority			= InThreadPriority;

	// Create a sync event to guarantee Init() function is called first
	threadInitSyncEvent = new CWindowsEvent( true );

	// Create a new thread
	handle = CreateThread( nullptr, InStackSize, &CWindowsThread::StaticMainProc, this, 0, ( LPDWORD )&threadId );

	// If it fails, clear all vars
	if ( !handle )
	{
		if ( InIsAutoDeleteRunnable )
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
	return handle != nullptr;
}

/*
==================
CWindowsThread::Kill
==================
*/
void CWindowsThread::Kill( bool InIsShouldWait /* = false */ )
{
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
		// 
		// IMPORTANT: It's not safe to just go and kill the thread with TerminateThread()
		// as it could have a mutex lock that's shared with a thread that's continuing to run, 
		// which would cause that other thread to dead-lock
		WaitForSingleObject( handle, INFINITE );
	}

	// Now clean up the thread handle so we don't leak
	CloseHandle( handle );
	handle = nullptr;

	// Delete the runnable if requested and we didn't shut down gracefully already
	if ( runnable && bAutoDeleteRunnable )
	{
		delete runnable;
		runnable = nullptr;
	}

	// Delete ourselves if requested and we didn't shut down gracefully already.
	// This check prevents a double-delete of self when we shut down gracefully
	if ( bAutoDeleteSelf )
	{
		delete this;
	}
}

/*
==================
CWindowsThread::StaticMainProc
==================
*/
DWORD STDCALL CWindowsThread::StaticMainProc( LPVOID InThis )
{
	CWindowsThread*		thisThread = ( CWindowsThread* )InThis;
	return thisThread->Run();
}

/*
==================
CWindowsThread::Run
==================
*/
uint32 CWindowsThread::Run()
{
	Assert( runnable );
	SetThreadPriority( threadPriority );

	// Initialize the runnable object
	bool	bInitReturn = runnable->Init();
	Assert( bInitReturn );

	// Initialization has completed, release the sync event
	threadInitSyncEvent->Trigger();

	// Now run the task that needs to be done
	uint32	exitCode = runnable->Run();

	// Allow any allocated resources to be cleaned up
	runnable->Exit();

	// Should we delete the runnable?
	if ( bAutoDeleteRunnable )
	{
		delete runnable;
		runnable = nullptr;
	}

	// Clean ourselves up without waiting
	// Now clean up the thread handle so we don't leak
	CloseHandle( handle );
	handle = nullptr;
	if ( bAutoDeleteSelf )
	{		
		delete this;
	}

	// Return from the thread with the exit code
	return exitCode;
}