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
#include "System/ThreadingBase.h"

FORCEINLINE uint32 appGetCurrentThreadId()
{
	return GetCurrentThreadId();
}

FORCEINLINE void appSetThreadPriority( void* InThreadHandle, EThreadPriority InThreadPriority )
{
	check( InThreadPriority == TP_Normal || InThreadPriority == TP_Low || InThreadPriority == TP_AboveNormal || InThreadPriority == TP_BelowNormal || InThreadPriority == TP_High || InThreadPriority == TP_Realtime );
	SetThreadPriority( InThreadHandle,
		InThreadPriority == TP_Low ? THREAD_PRIORITY_LOWEST :
		InThreadPriority == TP_BelowNormal ? THREAD_PRIORITY_BELOW_NORMAL :
		InThreadPriority == TP_AboveNormal ? THREAD_PRIORITY_NORMAL :
		InThreadPriority == TP_High ? THREAD_PRIORITY_ABOVE_NORMAL :
		InThreadPriority == TP_Realtime ? THREAD_PRIORITY_HIGHEST :
		THREAD_PRIORITY_NORMAL );
}

FORCEINLINE void appSleep( float InSeconds )
{
	Sleep( ( DWORD )( InSeconds * 1000.0 ) );
}

 /**
  * @ingroup WindowsPlatform
  * @brief Runnable thread for Windows
  */
class FRunnableThreadWindows : public FRunnableThread
{
public:
	/**
	 * Constructor
	 */
	FRunnableThreadWindows();

	/**
	 * Destructor
	 */
	virtual ~FRunnableThreadWindows();

	/**
	 * Tells the OS the preferred CPU to run the thread on. NOTE: Don't use
	 * this function unless you are absolutely sure of what you are doing
	 * as it can cause the application to run poorly by preventing the
	 * scheduler from doing its job well.
	 *
	 * @param[in] InProcessorNum The preferred processor for executing the thread on
	 */
	virtual void SetProcessorAffinity( uint32 InProcessorNum ) override;

	/**
	 * Set processor affinity mask
	 *
	 * @param[in] InProcessorMask Mask
	 */
	virtual void SetProcessorAffinityMask( uint32 InProcessorMask ) override;

	/**
	 * Tells the thread to either pause execution or resume depending on the
	 * passed in value.
	 *
	 * @param[in] InIsShouldPause Whether to pause the thread (true) or resume (false)
	 */
	virtual void Suspend( bool InIsShouldPause = true ) override;

	/**
	 * Tells the thread to exit. If the caller needs to know when the thread
	 * has exited, it should use the bShouldWait value and tell it how long
	 * to wait before deciding that it is deadlocked and needs to be destroyed.
	 * The implementation is responsible for calling Stop() on the runnable.
	 * NOTE: having a thread forcibly destroyed can cause leaks in TLS, etc.
	 *
	 * @param[in] InIsShouldWait If true, the call will wait for the thread to exit
	 * @return True if the thread exited graceful, false otherwise
	 */
	virtual bool Kill( bool InIsShouldWait = false ) override;

	/**
	 * Halts the caller until this thread is has completed its work.
	 */
	virtual void WaitForCompletion() override;

	/**
	 * Thread ID for this thread
	 *
	 * @return ID that was set by CreateThread
	 */
	virtual uint32 GetThreadID() const override;

	/**
	 * @brief Create thread.
	 *
	 * @param[in] InRunnable The runnable object to execute
	 * @param[in] InThreadName Name of the thread
	 * @param[in] InIsAutoDeleteSelf Whether to delete this object on exit
	 * @param[in] InIsAutoDeleteRunnable Whether to delete the runnable object on exit
	 * @param[in] InStackSize The size of the stack to create. 0 means use the current thread's stack size
	 * @param[in] InThreadPriority Tells the thread whether it needs to adjust its priority or not. Defaults to normal priority
	 *
	 * @return Return true if successfully create thread, else return false
	 */
	bool Create( FRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal );

private:
	/**
	 * Main function of thread
	 * 
	 * @param[in] InThis Pointer to current thread
	 */
	static DWORD STDCALL StaticMainProc( LPVOID InThis );

	/**
	 * Run thread
	 */
	uint32 Run();

	uint32					threadId;				/**< ID of thread */
	HANDLE					thread;					/**< Windows handle of thread */
	FRunnable*				runnable;				/**< Runnable object */
	FEvent*					threadInitSyncEvent;	/**< Sync event to make sure that Init() has been completed before allowing the main thread to continue */
	EThreadPriority			threadPriority;			/**< The priority to run the thread at */
	bool					isAutoDeleteSelf;		/**< Is need delete self*/
	bool					isAutoDeleteRunnable;	/**< Is need delete runnable object */
};

/**
 * @ingroup WindowsPlatform
 * @brief Thread factory Windows
 */
class FThreadFactoryWindows : public FThreadFactory
{
public:
	/**
	 * @brief Creates the thread with the specified stack size and thread priority.
	 *
	 * @param[in] InRunnable The runnable object to execute
	 * @param[in] InThreadName Name of the thread
	 * @param[in] InIsAutoDeleteSelf Whether to delete this object on exit
	 * @param[in] InIsAutoDeleteRunnable Whether to delete the runnable object on exit
	 * @param[in] InStackSize The size of the stack to create. 0 means use the current thread's stack size
	 * @param[in] InThreadPriority Tells the thread whether it needs to adjust its priority or not. Defaults to normal priority
	 *
	 * @return The newly created thread or nullptr if it failed
	 */
	virtual FRunnableThread* CreateThread( FRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal ) override;

	/**
	 * @brief Cleans up the specified thread object using the correct heap
	 *
	 * @param[in] InThread The thread object to destroy
	 */
	virtual void Destroy( FRunnableThread* InThread ) override;
};

/**
 * @ingroup WindowsPlatform
 * This is the Windows version of a critical section
 */
class FCriticalSection : public FSynchronize
{
public:
	/**
	 * Constructor that initializes the aggregated critical section
	 * <MSDN>: You can improve performance significantly by choosing a small spin count for a critical section
	 * of short duration. The heap manager uses a spin count of roughly 4000 for its per-heap critical sections.
	 * This gives great performance and scalability in almost all worst-case scenarios.
	 */
	enum { DEFAULT_SPIN_COUNT = 4000 };

	/**
	 * Constructor
	 * 
	 * @param[in] InDebugName Debug name
	 * @param[in] InSpinCount Spin count
	 */
	FORCEINLINE FCriticalSection( const tchar* InDebugName = nullptr, uint32 InSpinCount = DEFAULT_SPIN_COUNT )
	{
		InitializeCriticalSection(&criticalSection);
		SetCriticalSectionSpinCount(&criticalSection, InSpinCount);
	}

	/**
	 * Destructor
	 */
	FORCEINLINE ~FCriticalSection()
	{
		DeleteCriticalSection(&criticalSection);
	}

	/**
	 * Lock section
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
	 * Unlock section
	 */
	FORCEINLINE void Unlock()
	{
		LeaveCriticalSection( &criticalSection );
	}

private:
	CRITICAL_SECTION			criticalSection;			/**< The windows specific critical section */
};

/**
 * @ingroup Core
 *
 * This is the Windows version of an event
 */
class FEventWindows : public FEvent
{
public:
	/**
	 * Constructor
	 */
	FEventWindows();

	/**
	 * Destructor
	 */
	~FEventWindows();

	/**
	 * Creates the event. Manually reset events stay triggered until reset.
	 * Named events share the same underlying event.
	 *
	 * @param[in] InIsManualReset Whether the event requires manual reseting or not
	 * @param[in] InName Whether to use a commonly shared event or not. If so this is the name of the event to share.
	 * @return Returns true if the event was created, false otherwise
	 */
	virtual bool Create( bool InIsManualReset = false, const tchar* InName = nullptr ) override;

	/**
	 * Triggers the event so any waiting threads are activated
	 */
	virtual void Trigger() override;

	/**
	 * Resets the event to an untriggered (waitable) state
	 */
	virtual void Reset() override;

	/**
	 * Triggers the event and resets the triggered state (like auto reset)
	 */
	virtual void Pulse() override;

	/**
	 * Waits for the event to be triggered
	 *
	 * @param[in] InWaitTime Time in milliseconds to wait before abandoning the event (uint32)-1 is treated as wait infinite
	 * @return true if the event was signaled, false if the wait timed out
	 */
	virtual bool Wait( uint32 InWaitTime = ( uint32 )-1 ) override;

private:
	HANDLE				event;		/**< The handle to the event */
};

/**
 * @ingroup WindowsPlatform
 *
 * This is the Windows version of a semaphore object
 */
class FSemaphoreWindows : public FSemaphore
{
public:
	/**
	 * Constructor
	 */
	FSemaphoreWindows();

	/**
	 * Destructor
	 */
	~FSemaphoreWindows();

	/**
	 * Create semaphore
	 *
	 * @param[in] InMaxCount The maximum count for the semaphore object
	 * @param[in] InInitialCount The initial count for the semaphore object
	 * @param[in] InName Name of semaphore
	 * @return Return true if success, otherwise false
	 */
	virtual bool Create( uint32 InMaxCount, uint32 InInitialCount, const tchar* InName = nullptr ) override;

	/**
	 * Signal
	 *
	 * @return Return true if success, otherwise false
	 */
	virtual bool Signal() override;

	/**
	 * Post to semaphore
	 *
	 * @param[in] InCount The amount by which the semaphore object's current count is to be increased
	 * @return Return true if success, otherwise false
	 */
	virtual bool Post( uint32 InCount ) override;

	/**
	 * Try wait
	 *
	 * @return Return true if waited, otherwise false
	 */
	virtual bool TryWait() override;

	/**
	 * Wait infinite time
	 */
	virtual void Wait() override;

	/**
	 * Wait with seted time
	 *
	 * @param[in] InMilliseconds Wait time
	 * @return Return true if waited, otherwise false
	 */
	virtual bool WaitTimeoutMs( uint32 InMilliseconds ) override;

private:
	HANDLE			semaphore;				/**< The handle to the Semaphore */
};

/**
 * @ingroup WindowsPlatform
 *
 * This is the Windows factory for creating various synchronization objects.
 */
class FSynchronizeFactoryWindows : public FSynchronizeFactory
{
public:
	/**
	 * Creates a new critical section
	 *
	 * @return The new critical section object or nullptr otherwise
	 */
	virtual FCriticalSection* CreateCriticalSection() override;

	/**
	 * Creates a new event
	 *
	 * @param[in] InIsManualReset Whether the event requires manual reseting or not
	 * @param[in] InName Whether to use a commonly shared event or not. If so this is the name of the event to share.
	 * @return Returns the new event object if successful, NULL otherwise
	 */
	virtual FEvent* CreateSynchEvent( bool InIsManualReset = false, const tchar* InName = nullptr ) override;

	/**
	 * Creates a new semaphore object
	 *
	 * @param[in] InMaxCount
	 * @param[in] InInitialCount
	 * @return Returns the new semaphore object if successful, nullptr otherwise
	 */
	virtual FSemaphore* CreateSemaphore( uint32 InMaxCount, uint32 InInitialCount, const tchar* InName = nullptr ) override;

	/**
	 * Cleans up the specified synchronization object using the correct heap
	 *
	 * @param[in] InSynchObj The synchronization object to destroy
	 */
	virtual void Destroy( FSynchronize* InSynchObj ) override;
};

#endif // !WINDOWSTHREADING_H
