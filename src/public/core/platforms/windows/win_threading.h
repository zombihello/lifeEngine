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

#ifndef WIN_THREADING_H
#define WIN_THREADING_H

/**
 * @ingroup core
 * @brief Windows version of a thread mutex
 */
class CORE_INTERFACE CWindowsThreadMutex : public IThreadMutex
{
public:
	/**
	 * @brief Constructor
	 */
	CWindowsThreadMutex();

	/**
	 * @brief Destructor
	 */
	virtual ~CWindowsThreadMutex();

	/**
	 * @brief Lock
	 */
	virtual void Lock() override;

	/**
	 * @brief Lock
	 */
	virtual void Lock() const override;

	/**
	 * @brief Unlock
	 */
	virtual void Unlock() override;

	/**
	 * @brief Unlock
	 */
	virtual void Unlock() const override;

private:
	CRITICAL_SECTION	criticalSection;	/**< The windows specific critical section */
};

/**
 * @ingroup core
 * @brief Windows version of a thread event
 */
class CORE_INTERFACE CWindowsThreadEvent : public IThreadEvent
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param bManualReset	Whether the event requires manual reseting or not
	 * @param pName			Whether to use a commonly shared event or not. If so this is the name of the event to share
	 */
	CWindowsThreadEvent( bool bManualReset = false, const achar* pName = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~CWindowsThreadEvent();

	/**
	 * @brief Triggers the event so any waiting threads are activated
	 */
	virtual void Trigger() override;

	/**
	 * @brief Resets the event to an untriggered (waitable) state
	 */
	virtual void Reset() override;

	/**
	 * @brief Triggers the event and resets the triggered state (like auto reset)
	 */
	virtual void Pulse() override;

	/**
	 * @brief Waits for the event to be triggered
	 *
	 * @param waitTime	Time in milliseconds to wait before abandoning the event, -1 is treated as wait infinite
	 * @return Return TRUE if the event was signaled, FALSE if the wait timed out
	 */
	virtual bool Wait( uint32 waitTime = -1 ) override;

private:
	HANDLE	handle;		/**< The handle to the event */
};

/**
 * @ingroup core
 * @brief Windows version of a thread semaphore
 */
class CORE_INTERFACE CWindowsThreadSemaphore : public IThreadSemaphore
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param initialValue	The initial value for the semaphore object
	 * @param maxValue		The maximum value for the semaphore object
	 * @param pName			Name of semaphore
	 */
	CWindowsThreadSemaphore( uint32 initialValue, uint32 maxValue, const achar* pName = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~CWindowsThreadSemaphore();

	/**
	 * @brief Signal
	 * @return Return TRUE if success, otherwise FALSE
	 */
	virtual bool Signal() override;

	/**
	 * @brief Post to semaphore
	 *
	 * @param value		The amount by which the semaphore object's current value is to be increased
	 * @return Return TRUE if success, otherwise FALSE
	 */
	virtual bool Post( uint32 value ) override;

	/**
	 * @brief Wait infinite time
	 */
	virtual void Wait() override;

	/**
	 * @brief Wait with set time
	 *
	 * @param milliseconds	Wait time
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	virtual bool Wait( uint32 milliseconds ) override;

	/**
	 * @brief Try wait
	 * @return Return TRUE if waited, otherwise FALSE
	 */
	virtual bool TryWait() override;

private:
	HANDLE	handle;		/**< The handle to the Semaphore */
};

/**
 * @ingroup core
 * @brief Windows version of a base thread class
 */
class CORE_INTERFACE CWindowsThread : public IThread
{
public:
	/**
	 * @brief Constructor
	 */
	CWindowsThread();

	/**
	 * @brief Destructor
	 */
	virtual ~CWindowsThread();

	/**
	 * @brief Start thread running
	 * 
	 * @param stackSize		The size of the stack to create. 0 means use the current thread's stack size
	 * @return Return TRUE if successfully thread is started, otherwise returns FALSE
	 */
	virtual bool Start( uint32 stackSize = 0 ) override;

	/**
	 * @brief Tell the thread to stop
	 *
	 * @param bShouldWait	If TRUE, the call will wait for the thread to stop
	 * @param exitCode		Exit code of a thread
	 */
	virtual void Stop( bool bShouldWait = false, int32 exitCode = 0 ) override;

	/**
	 * @brief Suspend thread
	 * Tells the thread to either pause execution or resume depending on the passed in value
	 *
	 * @param bShouldPause	Whether to pause the thread (TRUE) or resume (FALSE)
	 */
	virtual void Suspend( bool bShouldPause = true ) override;

	/**
	 * @brief Halts the caller until this thread is has completed its work
	 */
	virtual void WaitForCompletion() override;

	/**
	 * @brief Is thread alive
	 * @return Return TRUE if thread has been created and hasn't yet exited, otherwise FALSE
	 */
	virtual bool IsAlive() const override;

	/**
	 * @brief Set thread name
	 * @param pName		Thread name
	 */
	virtual void SetName( const achar* pName ) override;

	/**
	 * @brief Get thread name
	 * @return Return thread name
	 */
	virtual const achar* GetName() const override;

	/**
	 * @brief Set thread priority
	 * @param priority	New thread priority
	 */
	virtual void SetPriority( EThreadPriority priority ) override;

	/**
	 * @brief Get thread priority
	 * @param Get thread priority
	 */
	virtual EThreadPriority GetPriority() const override;

	/**
	 * @brief Get OS thread handle
	 * @return Return OS thread handle
	 */
	virtual threadHandle_t GetThreadHandle() const override;

	/**
	 * @brief Get thread exit code
	 * @return Return the exit code that was returned from Run()
	 */
	virtual int32 GetExitCode() const override;

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
	virtual bool ThreadInit() override;

	/**
	 * @brief Stop a thread
	 * @note By default is empty
	 * 
	 * This is called if a thread is requested to terminate early.
	 */
	virtual void ThreadStop() override;

	/**
	 * @brief Exit a thread
	 * @note Called in the context of the aggregating thread
	 * @note By default is empty
	 * 
	 * Called to perform any cleanup
	 */
	virtual void ThreadExit() override;

private:
	/**
	 * @brief Main function of a thread
	 * @param pThis		Pointer to current thread
	 */
	static DWORD STDCALL ThreadMain( LPVOID pThis );

	int32					exitCode;				/**< The exit code that was returned from Run() */
	EThreadPriority			threadPriority;			/**< The priority to run the thread at */
	achar					name[64];				/**< Thread name */
	HANDLE					handle;					/**< Windows handle of thread */
	CWindowsThreadEvent*	pThreadInitSyncEvent;	/**< Sync event to make sure that Init() has been completed before allowing the main thread to continue */
};

#endif // !WIN_THREADING_H