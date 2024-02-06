/**
 * @file
 * @addtogroup Core Core
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

#include "Core/Core.h"
#include "Core/CoreDefines.h"
#include "Core/Misc/Types.h"

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

// Include platform specific inline functions
#if PLATFORM_WINDOWS
	#include "Core/Platforms/Windows/WindowsThreading.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup Core
 * @brief Game thread ID
 */
extern uint32			g_GameThreadId;

/**
 * @ingroup Core
 * @brief Is current thread is game
 *
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
	 * 
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 * 
	 * @return Return TRUE if initialization was successful, FALSE otherwise
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Run
	 * 
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 * 
	 * @return Return the exit code of the runnable object
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
	 * Called in the context of the aggregating thread to perform any cleanup
	 */
	virtual void Exit() = 0;
};

// Include platform specific implementation of interfaces
#if PLATFORM_WINDOWS
	#include "Core/Platforms/Windows/WindowsThreading.h"

	/**
	 * @ingroup Core
	 * @brief Cross-platform type mutex thread
	 */
	typedef CWindowsMutex		CThreadMutex;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type event thread
	 */
	typedef CWindowsEvent		CThreadEvent;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type semaphore thread
	 */
	typedef CWindowsSemaphore	CThreadSemaphore;

	/**
	 * @ingroup Core
	 * @brief Cross-platform type thread
	 */
	typedef CWindowsThread		CThread;
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
	 *
	 * @param InMutex	Mutex
	 */
	CScopeLock( CThreadMutex* InMutex )
		: syncObject( InMutex )
	{
		Assert( syncObject );
		syncObject->Lock();
	}

	/**
	 * @brief Constructor that performs a lock on the mutex
	 *
	 * @param InMutex	Mutex
	 */
	CScopeLock( CThreadMutex& InMutex )
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
	 *
	 * @param InScopeLock	Ignored
	 */
	CScopeLock( CScopeLock* InScopeLock )
		: syncObject( nullptr )
	{}

	/**
	 * @brief Assignment operator hidden on purpose
	 *
	 * @param InScopeLock	Ignored
	 */
	FORCEINLINE CScopeLock& operator=( CScopeLock& InScopeLock )
	{
		return *this;
	}

	CThreadMutex*	syncObject;		/**< The mutex to aggregate and scope manage */
};

#endif // !THREADING_H
