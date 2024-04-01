/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef THREADING_H
#define THREADING_H

#include "Core.h"
#include "CoreDefines.h"
#include "Misc/Types.h"

/**
 * @ingroup Core
 * @brief The list of enumerated thread priorities we support
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
 * @brief Atomically increments the value pointed to and returns that to the caller
 *
 * @param InValue	Pointer to value
 * @return Returns the incremented value
 */
FORCEINLINE int32 Sys_InterlockedIncrement( volatile int32* InValue );

/**
 * @ingroup Core
 * @brief Atomically decrements the value pointed to and returns that to the caller
 *
 * @param InValue	Pointer to value
 * @return Returns the minified value
 */
FORCEINLINE int32 Sys_InterlockedDecrement( volatile int32* InValue );

/**
 * @ingroup Core
 * @brief Atomically adds the amount to the value pointed to and returns the old value to the caller
 *
 * @param InValue		Value
 * @param InAmount		Amount
 * @return Return the old value to the caller
 */
FORCEINLINE int32 Sys_InterlockedAdd( volatile int32* InValue, int32 InAmount );

/**
 * @ingroup Core
 * @brief Atomically swaps two values returning the original value to the caller
 *
 * @param InValue		Value
 * @param InExchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int32 Sys_InterlockedExchange( volatile int32* InValue, int32 InExchange );

/**
 * @ingroup Core
 * @brief Atomically swaps two values returning the original value to the caller
 *
 * @param InValue		Value
 * @param InExchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int64 Sys_InterlockedExchange64( volatile int64* InValue, int64 InExchange );

/**
 * @ingroup Core
 * @brief Atomically compares the value to comparand and replaces with the exchange value if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comparand
 * @return Returns the original value
 */
FORCEINLINE int32 Sys_InterlockedCompareExchange( volatile int32* InDest, int32 InExchange, int32 InComperand );

/**
 * @ingroup Core
 * @brief Atomically compares the value to comparand and replaces with the exchange value if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comparand
 * @return Returns the original value
 */
FORCEINLINE int64 Sys_InterlockedCompareExchange64( volatile int64* InDest, int64 InExchange, int64 InComperand );

/**
 * @ingroup Core
 * @brief Atomically compares the pointer to comperand and replaces with the exchange pointer if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE void* Sys_InterlockedCompareExchangePointer( void** InDest, void* InExchange, void* InComperand );

/**
 * @ingroup Core
 * @brief Atomically or's the value pointed to with the value and returns that to the caller
 *
 * @param InDest	Distention
 * @param InValue	Value
 * @return Return that to the caller
 */
FORCEINLINE int32 Sys_InterlockedOr( volatile int32* InDest, int32 InValue );

/**
 * @ingroup Core
 * @brief Get current thread ID
 * @return Return current thread ID
 */
FORCEINLINE uint32 Sys_GetCurrentThreadId();

/**
 * @ingroup Core
 * @brief Offer a context switch
 */
FORCEINLINE void Sys_Yield();

/**
 * @ingroup Core
 * @brief Sleep this thread
 * @param InSeconds		Seconds of sleep
 */
FORCEINLINE void Sys_Sleep( float InSeconds );

/**
 * @ingroup Core
 * @brief Game thread ID
 */
extern uint32			g_GameThreadId;

/**
 * @ingroup Core
 * @brief Is current thread is the game thread
 * @return Return TRUE if called from the game thread, otherwise returns FALSE
 */
FORCEINLINE bool IsInGameThread()
{
	return ( g_GameThreadId == 0 ) || ( Sys_GetCurrentThreadId() == g_GameThreadId );
}

/**
 * @ingroup Core
 * @brief This is the base interface for runnable object in other thread
 */
class CRunnable
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CRunnable() {}

	/**
	 * @brief Initialize
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread
	 *
	 * @return Return TRUE if initialization was successful, FALSE otherwise
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Run
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful
	 *
	 * @return Return the exit code of the runnable object
	 */
	virtual uint32 Run() = 0;

	/**
	 * @brief Stop
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() = 0;

	/**
	 * @brief Exit
	 * Called in the context of the aggregating thread to perform any cleanup
	 */
	virtual void Exit() = 0;
};

/**
 * @ingroup Core
 * @brief Base runnable thread
 */
class CRunnableThread
{
public:
	/**
	 * @brief Constructor
	 */
	CRunnableThread();

	/**
	 * @brief Destructor
	 */
	virtual ~CRunnableThread() {}

	/**
	 * @brief Create thread
	 *
	 * @param InRunnable				The runnable object to execute
	 * @param InThreadName				Thread name
	 * @param InIsAutoDeleteSelf		Whether to delete this object on exit
	 * @param InIsAutoDeleteRunnable	Whether to delete the runnable object on exit
	 * @param InStackSize				The size of the stack to create. 0 means use the current thread's stack size
	 * @param InThreadPriority			Tells the thread whether it needs to adjust its priority or not. Defaults to normal priority
	 * @return Return created a new runnable thread, if failed returns NULL
	 */
	static CRunnableThread* Create( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal );

	/**
	 * @brief Set thread priority
	 * @param InPriority	New thread priority
	 */
	virtual void SetPriority( EThreadPriority InPriority ) = 0;

	/**
	 * @brief Suspend thread
	 * Tells the thread to either pause execution or resume depending on the passed in value
	 *
	 * @param InIsShouldPause	Whether to pause the thread (TRUE) or resume (FALSE)
	 */
	virtual void Suspend( bool InIsShouldPause = true ) = 0;

	/**
	 * @brief Tells the thread to exit
	 *
	 * @param InIsShouldWait	If TRUE, the call will wait infinitely for the thread to exit
	 * @param InExitCode		Exit code of a thread
	 */
	virtual void Kill( bool InIsShouldWait = false, int32 InExitCode = 0 ) = 0;

	/**
	 * @brief Halts the caller until this thread is has completed its work
	 */
	virtual void WaitForCompletion() = 0;

	/**
	 * @brief Is thread alive
	 * @return Return TRUE if thread has been created and hasn't yet exited, otherwise FALSE
	 */
	FORCEINLINE bool IsAlive() const
	{
		return threadID != ( uint32 )-1;
	}

	/**
	 * @brief Get thread priority
	 * @param Return thread priority
	 */
	FORCEINLINE EThreadPriority GetPriority() const
	{
		return threadPriority;
	}

	/**
	 * @brief Get ID of this thread
	 * @return Return ID of this thread
	 */
	FORCEINLINE uint32 GetThreadID() const
	{
		return threadID;
	}

	/**
	 * @brief Get thread exit code
	 * @return Return the exit code that was returned from CRunnable::Run()
	 */
	FORCEINLINE int32 GetExitCode() const
	{
		return exitCode;
	}

protected:
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
	virtual bool CreateInternal( CRunnable* InRunnable, const tchar* InThreadName, bool InIsAutoDeleteSelf = false, bool InIsAutoDeleteRunnable = false, uint32 InStackSize = 0, EThreadPriority InThreadPriority = TP_Normal ) = 0;

	int32				exitCode;		/**< The exit code that was returned from CRunnable::Run() */
	uint32				threadID;		/**< Thread ID */
	CRunnable*			runnable;		/**< The runnable object to execute on this thread */
	EThreadPriority		threadPriority;	/**< The priority to run the thread at */
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "WindowsThreading.h"

	/**
	 * @ingroup Core
	 * @brief Cross-platform type of mutex thread
	 */
	typedef CWindowsMutex				CMutex;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type of event thread
	 */
	typedef CWindowsEvent				CEvent;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type of semaphore thread
	 */
	typedef CWindowsSemaphore			CSemaphore;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type of runnable thread
	 */
	typedef CWindowsRunnableThread		CPlatformRunnableThread;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup Core
 * @brief This is a utility class that handles scope level locking
 *
 * @code
 *	{
 *		// Syncronize thread access to the following data
 *		CScopeLock		scopeLock( criticalSection );
 *		// Access data that is shared among multiple threads
 *		...
 *		// When ScopeLock goes out of scope, other threads can access data
 *	}
 * @endcode
 */
class CScopeLock
{
public:
	/**
	 * @brief Constructor that performs a lock on the mutex
	 * @param InMutex	Mutex
	 */
	CScopeLock( CMutex* InMutex )
		: syncObject( InMutex )
	{
		Assert( syncObject );
		syncObject->Lock();
	}

	/**
	 * @brief Constructor that performs a lock on the mutex
	 * @param InMutex	Mutex
	 */
	CScopeLock( CMutex& InMutex )
		: syncObject( &InMutex )
	{
		Assert( syncObject );
		syncObject->Lock();
	}

	/**
	 * @brief Destructor that performs a release on the mutex
	 */
	~CScopeLock()
	{
		Assert( syncObject );
		syncObject->Unlock();
	}

private:
	/**
	 * @brief Default constructor hidden on purpose
	 */
	CScopeLock() 
		: syncObject( nullptr )
	{}

	/**
	 * @brief Copy constructor hidden on purpose
	 * @param InScopeLock	Ignored
	 */
	CScopeLock( CScopeLock* InScopeLock )
		: syncObject( nullptr )
	{}

	/**
	 * @brief Assignment operator hidden on purpose
	 * @param InScopeLock	Ignored
	 */
	FORCEINLINE CScopeLock& operator=( CScopeLock& InScopeLock )
	{
		return *this;
	}

	CMutex*		syncObject;		/**< The mutex to aggregate and scope manage */
};

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "WindowsThreading.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !THREADING_H
