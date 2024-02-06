/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
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

#ifndef WINDOWSTHREADING_H
#define WINDOWSTHREADING_H

#include "Core/Core.h"

/**
 * @ingroup WindowsPlatform
 * @brief Windows version of a mutex
 */
class CWindowsMutex
{
public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CWindowsMutex()
	{
		/**
		 * Constructor that initializes the aggregated critical section
		 * <MSDN>: You can improve performance significantly by choosing a small spin count for a critical section
		 * of short duration. The heap manager uses a spin count of roughly 4000 for its per-heap critical sections.
		 * This gives great performance and scalability in almost all worst-case scenarios
		 */
		const int32 spinCount = 4000;

		InitializeCriticalSection( &criticalSection );
		SetCriticalSectionSpinCount( &criticalSection, spinCount );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CWindowsMutex()
	{
		DeleteCriticalSection( &criticalSection );
	}

	/**
	 * @brief Lock
	 */
	FORCEINLINE void Lock()
	{
		// Spin first before entering critical section, causing ring-0 transition and context switch.
		if ( TryEnterCriticalSection( &criticalSection ) == 0 )
		{
			EnterCriticalSection( &criticalSection );
		}
	}

	/**
	 * @brief Lock
	 */
	FORCEINLINE void Lock() const
	{
		const_cast<CWindowsMutex*>( this )->Lock();
	}

	/**
	 * @brief Unlock
	 */
	FORCEINLINE void Unlock()
	{
		LeaveCriticalSection( &criticalSection );
	}

	/**
	 * @brief Unlock
	 */
	FORCEINLINE void Unlock() const
	{
		const_cast<CWindowsMutex*>( this )->Unlock();
	}

private:
	CRITICAL_SECTION	criticalSection;	/**< The windows specific critical section */
};

/**
 * @ingroup WindowsPlatform
 * @brief Windows version of an event
 */
class CWindowsEvent
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InIsManualReset	Whether the event requires manual reseting or not
	 * @param InName			Whether to use a commonly shared event or not. If so this is the name of the event to share
	 */
	FORCEINLINE CWindowsEvent( bool InIsManualReset = false, const tchar* InName = nullptr )
		: handle( nullptr )
	{
		handle = CreateEvent( nullptr, InIsManualReset, 0, InName );
		AssertMsg( handle, TEXT( "Failed to create event (GetLastError 0x%X)" ), GetLastError() );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CWindowsEvent()
	{
		CloseHandle( handle );
	}

	/**
	 * @brief Triggers the event so any waiting threads are activated
	 */
	FORCEINLINE void Trigger()
	{
		SetEvent( handle );
	}

	/**
	 * @brief Resets the event to an untriggered (waitable) state
	 */
	FORCEINLINE void Reset()
	{
		ResetEvent( handle );
	}

	/**
	 * @brief Triggers the event and resets the triggered state (like auto reset)
	 */
	FORCEINLINE void Pulse()
	{
		PulseEvent( handle );
	}

	/**
	 * @brief Waits for the event to be triggered
	 *
	 * @param InWaitTime	Time in milliseconds to wait before abandoning the event, -1 is treated as wait infinite
	 * @return Return TRUE if the event was signaled, FALSE if the wait timed out
	 */
	FORCEINLINE bool Wait( uint32 InWaitTime = -1 )
	{
		return WaitForSingleObject( handle, InWaitTime ) == WAIT_OBJECT_0;
	}

private:
	HANDLE	handle;		/**< The handle to the event */
};

/**
 * @ingroup WindowsPlatform
 * @brief Windows version of an semaphore
 */
class CWindowsSemaphore
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InInitialValue	The initial value for the semaphore object
	 * @param InMaxValue		The maximum value for the semaphore object
	 * @param InName			Name of semaphore
	 */
	FORCEINLINE CWindowsSemaphore( uint32 InInitialValue, uint32 InMaxValue, const tchar* InName = nullptr )
	{
		AssertMsg( InMaxValue > 0, TEXT( "Invalid max value for semaphore" ) );
		AssertMsg( InInitialValue >= 0 && InInitialValue <= InMaxValue, TEXT( "Invalid initial value for semaphore" ) );
		handle = CreateSemaphore( nullptr, InInitialValue, InMaxValue, InName );
		AssertMsg( handle, TEXT( "Failed to create semaphore (GetLastError 0x%X)" ), GetLastError() );
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~CWindowsSemaphore()
	{
		CloseHandle( handle );
	}

	/**
	 * @brief Signal
	 * @return Return TRUE if success, otherwise FALSE
	 */
	FORCEINLINE bool Signal()
	{
		return Post( 1 );
	}

	/**
	 * @brief Post to semaphore
	 *
	 * @param InValue		The amount by which the semaphore object's current value is to be increased
	 * @return Return TRUE if success, otherwise FALSE
	 */
	FORCEINLINE bool Post( uint32 InValue )
	{
		bool	bResult = ReleaseSemaphore( handle, InValue, nullptr );
		AssertMsg( bResult, TEXT( "Failed to post semaphore (GetLastError 0x%X)" ), GetLastError() );
		return bResult;
	}

	/**
	 * @brief Wait infinite time
	 */
	FORCEINLINE void Wait()
	{;
		uint32	result = WaitForSingleObject( handle, INFINITE );
		Assert( result == WAIT_OBJECT_0 );
	}

	/**
	 * @brief Wait with set time
	 *
	 * @param InMilliseconds	Wait time
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	FORCEINLINE bool Wait( uint32 InMilliseconds )
	{
		uint32	result = WaitForSingleObject( handle, InMilliseconds );
		Assert( result != WAIT_FAILED );
		return result != WAIT_TIMEOUT;
	}

	/**
	 * @brief Try wait
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	FORCEINLINE bool TryWait()
	{
		return Wait( 0 );
	}

private:
	HANDLE	handle;		/**< The handle to the Semaphore */
};

/**
 * @ingroup WindowsPlatform
 * @brief Windows thread
 */
class CWindowsThread
{
public:
	/**
	 * @brief Constructor
	 */
	CWindowsThread();

	/**
	 * @brief Destructor
	 */
	~CWindowsThread();

	/**
	 * @brief Create thread
	 *
	 * @param InRunnable				The runnable object to execute
	 * @param InThreadName				Name of the thread
	 * @param InIsAutoDeleteSelf		Whether to delete this object on exit
	 * @param InIsAutoDeleteRunnable	Whether to delete the runnable object on exit
	 * @param InStackSize				The size of the stack to create. 0 means use the current thread's stack size
	 * @param InThreadPriority			Tells the thread whether it needs to adjust its priority or not. Defaults to normal priority
	 * @return Return TRUE if successfully create thread, otherwise returns FALSE
	 */
	bool Create( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal );

	/**
	 * @brief Suspend thread
	 * Tells the thread to either pause execution or resume depending on the
	 * passed in value
	 *
	 * @param InSsShouldPause	Whether to pause the thread (TRUE) or resume (FALSE)
	 */
	FORCEINLINE void Suspend( bool InIsShouldPause = true )
	{
		if ( !handle )
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

	/**
	 * @brief Tell the thread to exit
	 * @param InIsShouldWait	If TRUE, the call will wait for the thread to exit
	 */
	void Kill( bool InIsShouldWait = false );

	/**
	 * @brief Halts the caller until this thread is has completed its work
	 */
	FORCEINLINE void WaitForCompletion()
	{
		if ( handle )
		{
			// Block until this thread exits
			WaitForSingleObject( handle, INFINITE );
		}
	}

	/**
	 * @brief Set thread priority
	 * @param InThreadPriority	New thread priority
	 */
	FORCEINLINE void SetThreadPriority( EThreadPriority InThreadPriority )
	{
		if ( handle )
		{
			Sys_SetThreadPriority( handle, InThreadPriority );
		}
	}

	/**
	 * @brief Get Id for this thread
	 * @return Return thread Id of this thread
	 */
	FORCEINLINE uint32 GetThreadID() const
	{
		return threadId;
	}

private:
	/**
	 * @brief Main function of thread
	 * @param InThis	Pointer to current thread
	 */
	static DWORD STDCALL StaticMainProc( LPVOID InThis );

	/**
	 * @brief Run thread
	 */
	uint32 Run();

	uint32				threadId;				/**< ID of the thread */
	HANDLE				handle;					/**< Windows handle of thread */
	CRunnable*			runnable;				/**< Runnable object */
	CWindowsEvent*		threadInitSyncEvent;	/**< Sync event to make sure that Init() has been completed before allowing the main thread to continue */
	EThreadPriority		threadPriority;			/**< The priority to run the thread at */
	bool				bAutoDeleteSelf;		/**< Is need delete self*/
	bool				bAutoDeleteRunnable;	/**< Is need delete runnable object */
};

#endif // !WINDOWSTHREADING_H
