/**
 * @file
 * @addtogroup core core
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

#ifndef THREADING_H
#define THREADING_H

#include "core/core.h"
#include "core/debug.h"

/**
 * @ingroup core
 * @brief The list of enumerated thread priorities we support
 */
enum EThreadPriority
{
	THREAD_PRIOR_LOW,				/**< Low */
	THREAD_PRIOR_BELOW_NORMAL,		/**< Below normal */
	THREAD_PRIOR_NORMAL,			/**< Normal */
	THREAD_PRIOR_ABOVE_NORMAL,		/**< Above normal */
	THREAD_PRIOR_HIGH,				/**< High */
	THREAD_PRIOR_REALTIME			/**< Realtime */
};

/**
 * @ingroup core
 * @brief Atomically increments the value pointed to and returns that to the caller
 * @note Need implement on each platform
 * 
 * @param pValue	Pointer to value
 * @return Returns the incremented value
 */
FORCEINLINE int32 Sys_InterlockedIncrement( volatile int32* pValue );

/**
 * @ingroup core
 * @brief Atomically decrements the value pointed to and returns that to the caller
 * @note Need implement on each platform
 * 
 * @param pValue	Pointer to value
 * @return Returns the minified value
 */
FORCEINLINE int32 Sys_InterlockedDecrement( volatile int32* pValue );

/**
 * @ingroup core
 * @brief Atomically adds the amount to the value pointed to and returns the old value to the caller
 * @note Need implement on each platform
 * 
 * @param pValue	Pointer to value
 * @param amount	Amount
 * @return Return the old value to the caller
 */
FORCEINLINE int32 Sys_InterlockedAdd( volatile int32* pValue, int32 amount );

/**
 * @ingroup core
 * @brief Atomically swaps two values returning the original value to the caller
 * @note Need implement on each platform
 * 
 * @param pValue	Pointer to value
 * @param exchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int32 Sys_InterlockedExchange( volatile int32* pValue, int32 exchange );

/**
 * @ingroup core
 * @brief Atomically swaps two values returning the original value to the caller
 * @note Need implement on each platform
 * 
 * @param pValue	Pointer to value
 * @param exchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int64 Sys_InterlockedExchange64( volatile int64* pValue, int64 exchange );

/**
 * @ingroup core
 * @brief Atomically compares the value to comperand and replaces with the exchange value if they are equal and returns the original value
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to distention
 * @param exchange		Exchange
 * @param comperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE int32 Sys_InterlockedCompareExchange( volatile int32* pDest, int32 exchange, int32 comperand );

/**
 * @ingroup core
 * @brief Atomically compares the value to comperand and replaces with the exchange value if they are equal and returns the original value
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to distention
 * @param exchange		Exchange
 * @param comperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE int64 Sys_InterlockedCompareExchange64( volatile int64* pDest, int64 exchange, int64 comperand );

/**
 * @ingroup core
 * @brief Atomically compares the pointer to comperand and replaces with the exchange pointer if they are equal and returns the original value
 * @note Need implement on each platform
 *
 * @param pDest			Pointer to distention
 * @param pExchange		Pointer to exchange
 * @param pComperand	Pointer to comperand
 * @return Returns the original value
 */
FORCEINLINE void* Sys_InterlockedCompareExchangePointer( void** pDest, void* pExchange, void* pComperand );

/**
 * @ingroup core
 * @brief Atomically or's the value pointed to with the value and returns that to the caller
 * @note Need implement on each platform
 * 
 * @param pDest		Pointer to distention
 * @param value		Value
 * @return Return that to the caller
 */
FORCEINLINE int32 Sys_InterlockedOr( volatile int32* pDest, int32 value );

/**
 * @ingroup core
 * @brief Offer a context switch
 * @note Need implement on each platform
 */
FORCEINLINE void Sys_Yield();

/**
 * @ingroup core
 * @brief Sleep this thread
 * @note Need implement on each platform
 * 
 * @param seconds	Seconds of sleep
 */
FORCEINLINE void Sys_Sleep( float seconds );

/**
 * @ingroup core
 * @brief Is current thread is the main thread
 * @return Return TRUE if called from the main thread, otherwise returns FALSE
 */
CORE_INTERFACE bool Sys_IsInMainThread();

/**
 * @ingroup core
 * @brief Thread mutex interface
 */
class CORE_CLASS IThreadMutex
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IThreadMutex() {}

	/**
	 * @brief Lock
	 */
	virtual void Lock() = 0;

	/**
	 * @brief Lock
	 */
	virtual void Lock() const = 0;

	/**
	 * @brief Unlock
	 */
	virtual void Unlock() = 0;

	/**
	 * @brief Unlock
	 */
	virtual void Unlock() const = 0;
};

/**
 * @ingroup core
 * @brief Thread event interface
 */
class CORE_CLASS IThreadEvent
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IThreadEvent() {}

	/**
	 * @brief Triggers the event so any waiting threads are activated
	 */
	virtual void Trigger() = 0;

	/**
	 * @brief Resets the event to an untriggered (waitable) state
	 */
	virtual void Reset() = 0;

	/**
	 * @brief Triggers the event and resets the triggered state (like auto reset)
	 */
	virtual void Pulse() = 0;

	/**
	 * @brief Waits for the event to be triggered
	 *
	 * @param waitTime	Time in milliseconds to wait before abandoning the event, -1 is treated as wait infinite
	 * @return Return TRUE if the event was signaled, FALSE if the wait timed out
	 */
	virtual bool Wait( uint32 waitTime = -1 ) = 0;
};

/**
 * @ingroup core
 * @brief Thread semaphore interface
 */
class CORE_CLASS IThreadSemaphore
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IThreadSemaphore() {}

	/**
	 * @brief Signal
	 * @return Return TRUE if success, otherwise FALSE
	 */
	virtual bool Signal() = 0;

	/**
	 * @brief Post to semaphore
	 *
	 * @param value		The amount by which the semaphore object's current value is to be increased
	 * @return Return TRUE if success, otherwise FALSE
	 */
	virtual bool Post( uint32 value ) = 0;

	/**
	 * @brief Wait infinite time
	 */
	virtual void Wait() = 0;

	/**
	 * @brief Wait with set time
	 *
	 * @param milliseconds	Wait time
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	virtual bool Wait( uint32 milliseconds ) = 0;

	/**
	 * @brief Try wait
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	virtual bool TryWait() = 0;
};

/**
 * @ingroup core
 * @brief Thread interface
 */
class CORE_CLASS IThread
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~IThread() {}

	/**
	 * @brief Start thread running
	 * 
	 * @param stackSize		The size of the stack to create. 0 means use the current thread's stack size
	 * @return Return TRUE if successfully thread is started, otherwise returns FALSE
	 */
	virtual bool Start( uint32 stackSize = 0 ) = 0;

	/**
	 * @brief Tell the thread to stop
	 * 
	 * @param bShouldWait	If TRUE, the call will wait for the thread to stop
	 * @param exitCode		Exit code of a thread
	 */
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) = 0;

	/**
	 * @brief Suspend thread
	 * Tells the thread to either pause execution or resume depending on the passed in value
	 *
	 * @param bShouldPause	Whether to pause the thread (TRUE) or resume (FALSE)
	 */
	virtual void Suspend( bool bShouldPause = true ) = 0;

	/**
	 * @brief Halts the caller until this thread is has completed its work
	 */
	virtual void WaitForCompletion() = 0;

	/**
	 * @brief Is thread alive
	 * @return Return TRUE if thread has been created and hasn't yet exited, otherwise FALSE
	 */
	virtual bool IsAlive() const = 0;

	/**
	 * @brief Set thread name
	 * @param pName		Thread name
	 */
	virtual void SetName( const achar* pName ) = 0;

	/**
	 * @brief Get thread name
	 * @return Return thread name
	 */
	virtual const achar* GetName() const = 0;

	/**
	 * @brief Set thread priority
	 * @param priority	New thread priority
	 */
	virtual void SetPriority( EThreadPriority priority ) = 0;

	/**
	 * @brief Get thread priority
	 * @param Get thread priority
	 */
	virtual EThreadPriority GetPriority() const = 0;

	/**
	 * @brief Get OS thread handle
	 * @return Return OS thread handle
	 */
	virtual threadHandle_t GetThreadHandle() const = 0;

	/**
	 * @brief Get thread exit code
	 * @return Return the exit code that was returned from Run()
	 */
	virtual int32 GetExitCode() const = 0;

protected:
	/**
	 * @brief Initialize a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty and always returns TRUE
	 * 
	 * Allows per thread initialization
	 *
	 * @return Return TRUE if initialization was successful, FALSE otherwise
	 */
	virtual bool ThreadInit() = 0;

	/**
	 * @brief Run a thread
	 * @note Called in the context of the aggregating thread
	 * 
	 * This is where all per thread work is done. This is only called
	 * if the initialization was successful
	 *
	 * @return Return the exit code of the thread
	 */
	virtual uint32 ThreadRun() = 0;

	/**
	 * @brief Stop a thread
	 * @note By default is empty
	 * 
	 * This is called if a thread is requested to terminate early.
	 */
	virtual void ThreadStop() = 0;

	/**
	 * @brief Exit a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty
	 * 
	 * Called to perform any cleanup
	 */
	virtual void ThreadExit() = 0;
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_threading.h"

	/**
	 * @ingroup core
	 * @brief Cross-platform type mutex thread
	 */
	typedef CWindowsThreadMutex			CThreadMutex;

	/**
	 * @ingroup core
	 * @brief Cross-platform type event thread
	 */
	typedef CWindowsThreadEvent			CThreadEvent;

	/**
	 * @ingroup core
	 * @brief Cross-platform type semaphore thread
	 */
	typedef CWindowsThreadSemaphore		CThreadSemaphore;

	/**
	 * @ingroup core
	 * @brief Cross-platform type thread
	 */
	typedef CWindowsThread				CThread;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup core
 * @brief This is a utility class that handles scope level locking
 *
 * @code
 *	{
 *		// Syncronize thread access to the following data
 *		CScopeLock		scopeLock( mutex );
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
	 *
	 * @param pMutex	Mutex
	 */
	CScopeLock( CThreadMutex* pMutex )
		: pSyncObject( pMutex )
	{
		Assert( pSyncObject );
		pSyncObject->Lock();
	}

	/**
	 * @brief Constructor that performs a lock on the mutex
	 *
	 * @param mutex		Mutex
	 */
	CScopeLock( CThreadMutex& mutex )
		: pSyncObject( &mutex )
	{
		Assert( pSyncObject );
		pSyncObject->Lock();
	}

	/**
	 * @brief Destructor that performs a release on the mutex
	 */
	~CScopeLock()
	{
		Assert( pSyncObject );
		pSyncObject->Unlock();
	}

private:
	/**
	 * @brief Default constructor hidden on purpose
	 */
	CScopeLock() 
		: pSyncObject( nullptr )
	{}

	/**
	 * @brief Copy constructor hidden on purpose
	 *
	 * @param scopeLock		Ignored
	 */
	CScopeLock( CScopeLock& scopeLock )
		: pSyncObject( nullptr )
	{}

	/**
	 * @brief Assignment operator hidden on purpose
	 *
	 * @param scopeLock		Ignored
	 */
	FORCEINLINE CScopeLock& operator=( CScopeLock& scopeLock )
	{
		return *this;
	}

	CThreadMutex*	pSyncObject;		/**< The mutex to aggregate and scope manage */
};

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_threading.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

#endif // !THREADING_H