/**
 * @file
 * @addtogroup WindowsPlatform Windows platform
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSTHREADING_H
#define WINDOWSTHREADING_H

#include "Misc/Types.h"

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
		// Spin first before entering critical section, causing ring-0 transition and context switch
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
	 * @param InName			Semaphore name
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
	 * @param InValue	The amount by which the semaphore object's current value is to be increased
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
  * @brief Runnable thread for Windows
  */
class CWindowsRunnableThread : public CRunnableThread
{
public:
	/**
	 * @brief Constructor
	 */
	CWindowsRunnableThread();

	/**
	 * @brief Destructor
	 */
	virtual ~CWindowsRunnableThread();

	/**
	 * @brief Set thread priority
	 * @param InPriority	New thread priority
	 */
	virtual void SetPriority( EThreadPriority InPriority ) override;

	/**
	 * @brief Suspend thread
	 * Tells the thread to either pause execution or resume depending on the passed in value
	 *
	 * @param InIsShouldPause	Whether to pause the thread (TRUE) or resume (FALSE)
	 */
	virtual void Suspend( bool InIsShouldPause = true ) override;

	/**
	 * @brief Tells the thread to exit
	 *
	 * @param InIsShouldWait	If TRUE, the call will wait infinitely for the thread to exit
	 * @param InExitCode		Exit code of a thread
	 */
	virtual void Kill( bool InIsShouldWait = false, int32 InExitCode = 0 ) override;

	/**
	 * @brief Halts the caller until this thread is has completed its work
	 */
	virtual void WaitForCompletion() override;

private:
	/**
	 * @brief Internal function for create a thread
	 *
	 * @param InRunnable				The runnable object to execute
	 * @param InThreadName				Thread name
	 * @param InIsAutoDeleteSelf		Whether to delete this object on exit
	 * @param InIsAutoDeleteRunnable	Whether to delete the runnable object on exit
	 * @param InStackSize				The size of the stack to create. 0 means use the current thread's stack size
	 * @param InThreadPriority			Tells the thread whether it needs to adjust its priority or not. Defaults to normal priority
	 * @return Return TRUE if successfully create thread, otherwise returns FALSE
	 */
	virtual bool CreateInternal( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal ) override;

	/**
	 * @brief Main thread function 
	 * @param InThis	Pointer to current thread
	 */
	static DWORD STDCALL StaticMainProc( LPVOID InThis );

	/**
	 * @brief Run thread
	 * @return Return exit code
	 */
	uint32 Run();

	HANDLE			handle;					/**< Windows handle of thread */
	CWindowsEvent*	threadInitSyncEvent;	/**< Sync event to make sure that Init() has been completed before allowing the main thread to continue */
	bool			bAutoDeleteSelf;		/**< Is need delete self at the end */
	bool			bAutoDeleteRunnable;	/**< Is need delete runnable object at the end */
};

#endif // !WINDOWSTHREADING_H
