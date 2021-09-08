/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef THREADINGBASE_H
#define THREADINGBASE_H

#include "CoreDefines.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * ID of game thread
 */
extern uint32			GGameThreadId;

/**
 * @ingroup Core
 * @brief Get ID of current thread
 *
 * @return Return ID of current thread
 */
extern FORCEINLINE uint32 appGetCurrentThreadId();

/**
 * @ingroup Core
 * Is current thread is game
 * 
 * @return True if called from the game thread
 */
FORCEINLINE bool IsInGameThread()
{
	// if the game or rendering threads are uninitialized, then we probably are calling this VERY early before other threads will have started up, 
	// or we're running the commandlet with rendering thread or we're running in single threaded mode
	return ( GGameThreadId == 0 ) || ( appGetCurrentThreadId() == GGameThreadId );
}

/**
 * @ingroup Core
 * The list of enumerated thread priorities we support
 */
enum EThreadPriority
{
	TP_Low,				/**< Low */
	TP_BelowNormal,		/**< Below normal */
	TP_Normal,			/**< Normal */
	TP_AboveNormal,		/**< Above normal */
	TP_High,			/**< High */
	TP_Realtime			/**< Realtime */
};

/**
 * @ingroup Core
 * Set thread priority
 */
extern FORCEINLINE void appSetThreadPriority( void* InThreadHandle, EThreadPriority InThreadPriority );

/**
 * @ingroup Core
 * Sleep this thread
 * 
 * @param[in] InSeconds Seconds of sleep
 */
extern FORCEINLINE void appSleep( float InSeconds );

/**
 * @ingroup Core
 * @brief This is the base interface for "runnable" object.
 * 
 * This is the base interface for "runnable" object. A runnable object is an
 * object that is "run" on an arbitrary thread. The call usage pattern is
 * Init(), Run(), Exit(). The thread that is going to "run" this object always
 * uses those calling semantics. It does this on the thread that is created so
 * that any thread specific uses (TLS, etc.) are available in the contexts of
 * those calls. A "runnable" does all initialization in Init(). If
 * initialization fails, the thread stops execution and returns an error code.
 * If it succeeds, Run() is called where the real threaded work is done. Upon
 * completion, Exit() is called to allow correct clean up.
 */
class FRunnable
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FRunnable() {}

	/**
	 * @brief Initialize
	 * 
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 * 
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Run
	 * 
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 * 
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() = 0;

	/**
	 * @brief Stop
	 * 
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() = 0;

	/**
	 * @brief Exit
	 * 
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() = 0;
};

/**
 * @ingroup Core
 * @brief Base runnable thread
 * 
 * This is the base interface for all runnable thread classes. It specifies the
 * methods used in managing its life cycle.
 */
class FRunnableThread
{
public:
	/**
	 * Destructor
	 */
	virtual ~FRunnableThread() {}

	/**
	 * Tells the OS the preferred CPU to run the thread on. NOTE: Don't use
	 * this function unless you are absolutely sure of what you are doing
	 * as it can cause the application to run poorly by preventing the
	 * scheduler from doing its job well.
	 *
	 * @param[in] InProcessorNum The preferred processor for executing the thread on
	 */
	virtual void SetProcessorAffinity( uint32 InProcessorNum ) = 0;

	/**
	 * Set processor affinity mask
	 * 
	 * @param[in] InProcessorMask Mask
	 */
	virtual void SetProcessorAffinityMask( uint32 InProcessorMask ) = 0;

	/**
	 * Tells the thread to either pause execution or resume depending on the
	 * passed in value.
	 *
	 * @param[in] InIsShouldPause Whether to pause the thread (true) or resume (false)
	 */
	virtual void Suspend( bool InIsShouldPause = true ) = 0;

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
	virtual bool Kill( bool InIsShouldWait = false ) = 0;

	/**
	 * Halts the caller until this thread is has completed its work.
	 */
	virtual void WaitForCompletion() = 0;

	/**
	 * Thread ID for this thread
	 *
	 * @return ID that was set by CreateThread
	 */
	virtual uint32 GetThreadID() const = 0;
};

/**
 * @ingroup Core
 * 
 * This is the factory interface for creating threads. Each platform must
 * implement this with all the correct platform semantics
 */
class FThreadFactory
{
public:
	/**
	 * Destructor
	 */
	virtual ~FThreadFactory() {}

	/**
	 * Creates the thread with the specified stack size and thread priority.
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
	virtual FRunnableThread* CreateThread( FRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal ) = 0;

	/**
	 * Cleans up the specified thread object using the correct heap
	 *
	 * @param[in] InThread The thread object to destroy
	 */
	virtual void Destroy( FRunnableThread* InThread ) = 0;
};

/*
 * @ingroup Core
 * Global factory for creating threads
 */
extern FThreadFactory*			GThreadFactory;

/**
 * @ingroup Core
 * Simple base class for polymorphic cleanup
 */
class FSynchronize
{
public:
	/**
	 * Destructor
	 */
	virtual ~FSynchronize() {}
};

/**
 * Forward declaration for a platform specific implementation
 */
class FCriticalSection;

/**
 * @ingroup Core
 * 
 * This class is the abstract representation of a waitable event. It is used
 * to wait for another thread to signal that it is ready for the waiting thread
 * to do some work. Very useful for telling groups of threads to exit.
 */
class FEvent : public FSynchronize
{
public:
	/**
	 * Destructor
	 */
	virtual ~FEvent() {}

	/**
	 * Creates the event. Manually reset events stay triggered until reset.
	 * Named events share the same underlying event.
	 *
	 * @param[in] InIsManualReset Whether the event requires manual reseting or not
	 * @param[in] InName Whether to use a commonly shared event or not. If so this is the name of the event to share.
	 * @return Returns true if the event was created, false otherwise
	 */
	virtual bool Create( bool InIsManualReset = false, const tchar* InName = nullptr ) = 0;

	/**
	 * Triggers the event so any waiting threads are activated
	 */
	virtual void Trigger() = 0;

	/**
	 * Resets the event to an untriggered (waitable) state
	 */
	virtual void Reset() = 0;

	/**
	 * Triggers the event and resets the triggered state (like auto reset)
	 */
	virtual void Pulse() = 0;

	/**
	 * Waits for the event to be triggered
	 *
	 * @param[in] InWaitTime Time in milliseconds to wait before abandoning the event (uint32)-1 is treated as wait infinite
	 * @return true if the event was signaled, false if the wait timed out
	 */
	virtual bool Wait( uint32 InWaitTime = ( uint32 )-1 ) = 0;
};

/**
 * @ingroup Core
 * 
 * This class is the abstract representation of a semaphore object
 */
class FSemaphore : public FSynchronize
{
public:
	/**
	 * Destructor
	 */
	virtual ~FSemaphore() {}

	/**
	 * Create semaphore
	 *
	 * @param[in] InMaxCount The maximum count for the semaphore object
	 * @param[in] InInitialCount The initial count for the semaphore object
	 * @param[in] InName Name of semaphore
	 * @return Return true if success, otherwise false
	 */
	virtual bool Create( uint32 InMaxCount, uint32 InInitialCount, const tchar* InName = nullptr ) = 0;

	/**
	 * Signal
	 * 
	 * @return Return true if success, otherwise false
	 */
	virtual bool Signal() = 0;

	/**
	 * Post to semaphore
	 * 
	 * @param[in] InCount The amount by which the semaphore object's current count is to be increased
	 * @return Return true if success, otherwise false
	 */
	virtual bool Post( uint32 InCount ) = 0;

	/**
	 * Try wait
	 * 
	 * @return Return true if waited, otherwise false
	 */
	virtual bool TryWait() = 0;

	/**
	 * Wait infinite time
	 */
	virtual void Wait() = 0;

	/**
	 * Wait with seted time
	 * 
	 * @param[in] InMilliseconds Wait time
	 * @return Return true if waited, otherwise false 
	 */
	virtual bool WaitTimeoutMs( uint32 InMilliseconds ) = 0;
};

/**
 * @ingroup Core
 * 
 * This is the factory interface for creating various synchronization objects.
 * It is overloaded on a per platform basis to hide how each platform creates
 * the various synchronization objects. NOTE: The malloc used by it must be
 * thread safe
 */
class FSynchronizeFactory
{
public:
	/**
	 * Destructor
	 */
	virtual ~FSynchronizeFactory() {}

	/**
	 * Creates a new critical section
	 *
	 * @return The new critical section object or nullptr otherwise
	 */
	virtual FCriticalSection* CreateCriticalSection() = 0;

	/**
	 * Creates a new event
	 *
	 * @param[in] InIsManualReset Whether the event requires manual reseting or not
	 * @param[in] InName Whether to use a commonly shared event or not. If so this is the name of the event to share.
	 * @return Returns the new event object if successful, NULL otherwise
	 */
	virtual FEvent* CreateSynchEvent( bool InIsManualReset = false, const tchar* InName = nullptr ) = 0;

	/**
	 * Creates a new semaphore object
	 *
	 * @param[in] InMaxCount
	 * @param[in] InInitialCount
	 * @return Returns the new semaphore object if successful, nullptr otherwise
	 */
	virtual FSemaphore* CreateSemaphore( uint32 InMaxCount, uint32 InInitialCount, const tchar* InName = nullptr ) = 0;


	/**
	 * Cleans up the specified synchronization object using the correct heap
	 *
	 * @param[in] InSynchObj The synchronization object to destroy
	 */
	virtual void Destroy( FSynchronize* InSynchObj ) = 0;
};

/*
 * @ingroup Core
 * Global factory for creating synchronization objects
 */
extern FSynchronizeFactory*				GSynchronizeFactory;

#if PLATFORM_WINDOWS
	#include "WindowsThreading.h"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !THREADINGBASE_H
