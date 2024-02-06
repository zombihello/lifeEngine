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

#ifndef WINDOWSTHREADING_INL
#define WINDOWSTHREADING_INL

/**
 * @ingroup WindowsPlatform
 * @brief Atomically increments the value pointed to and returns that to the caller
 *
 * @param InValue	Pointer to value
 * @return Returns the incremented value
 */
FORCEINLINE int32 Sys_InterlockedIncrement( volatile int32* InValue )
{
	return ( int32 )InterlockedIncrement( ( LPLONG )InValue );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically decrements the value pointed to and returns that to the caller
 *
 * @param InValue	Pointer to value
 * @return Returns the minified value
 */
FORCEINLINE int32 Sys_InterlockedDecrement( volatile int32* InValue )
{
	return ( int32 )InterlockedDecrement( ( LPLONG )InValue );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically adds the amount to the value pointed to and returns the old
 * value to the caller
 *
 * @param InValue		Value
 * @param InAmount		Amount
 * @return Return the old value to the caller
 */
FORCEINLINE int32 Sys_InterlockedAdd( volatile int32* InValue, int32 InAmount )
{
	return ( int32 )InterlockedExchangeAdd( ( LPLONG )InValue, ( LONG )InAmount );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically swaps two values returning the original value to the caller
 *
 * @param InValue		Value
 * @param InExchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int32 Sys_InterlockedExchange( volatile int32* InValue, int32 InExchange )
{
	return ( int32 )InterlockedExchange( ( LPLONG )InValue, ( LONG )InExchange );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically swaps two values returning the original value to the caller
 *
 * @param InValue		Value
 * @param InExchange	Exchange
 * @return Returning the original value to the caller
 */
FORCEINLINE int64 Sys_InterlockedExchange64( volatile int64* InValue, int64 InExchange )
{
	return ( int64 )InterlockedExchange64( InValue, InExchange );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically compares the value to comperand and replaces with the exchange
 * value if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE int32 Sys_InterlockedCompareExchange( volatile int32* InDest, int32 InExchange, int32 InComperand )
{
	return ( int32 )InterlockedCompareExchange( ( LPLONG )InDest, ( LONG )InExchange, ( LONG )InComperand );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically compares the value to comperand and replaces with the exchange
 * value if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE int64 Sys_InterlockedCompareExchange64( volatile int64* InDest, int64 InExchange, int64 InComperand )
{
	return ( int64 )InterlockedCompareExchange64( InDest, InExchange, InComperand );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically compares the pointer to comperand and replaces with the exchange
 * pointer if they are equal and returns the original value
 *
 * @param InDest			Distention
 * @param InExchange		Exchange
 * @param InComperand		Comperand
 * @return Returns the original value
 */
FORCEINLINE void* Sys_InterlockedCompareExchangePointer( void** InDest, void* InExchange, void* InComperand )
{
	return InterlockedCompareExchangePointer( InDest, InExchange, InComperand );
}

/**
 * @ingroup WindowsPlatform
 * @brief Atomically or's the value pointed to with the value and returns that to the caller
 *
 * @param InDest	Distention
 * @param InValue	Value
 * @return Return that to the caller
 */
FORCEINLINE int32 Sys_InterlockedOr( volatile int32* InDest, int32 InValue )
{
	return ( int32 )InterlockedOr( ( LPLONG )InDest, ( LONG )InValue );
}

/**
 * @ingroup WindowsPlatform
 * @brief Get Id of current thread
 *
 * @return Return Id of current thread
 */
FORCEINLINE uint32 Sys_GetCurrentThreadId()
{
	return GetCurrentThreadId();
}

/**
 * @ingroup WindowsPlatform
 * @brief Set thread priority
 * 
 * @param InThreadHandle	OS thread handle
 * @param InThreadPriority	Thread priority
 */
FORCEINLINE void Sys_SetThreadPriority( void* InThreadHandle, EThreadPriority InThreadPriority )
{
	Assert( InThreadPriority == TP_Normal || InThreadPriority == TP_Low || InThreadPriority == TP_AboveNormal || InThreadPriority == TP_BelowNormal || InThreadPriority == TP_High || InThreadPriority == TP_Realtime );
	SetThreadPriority( InThreadHandle,
		InThreadPriority == TP_Low ? THREAD_PRIORITY_LOWEST :
		InThreadPriority == TP_BelowNormal ? THREAD_PRIORITY_BELOW_NORMAL :
		InThreadPriority == TP_AboveNormal ? THREAD_PRIORITY_NORMAL :
		InThreadPriority == TP_High ? THREAD_PRIORITY_ABOVE_NORMAL :
		InThreadPriority == TP_Realtime ? THREAD_PRIORITY_HIGHEST :
		THREAD_PRIORITY_NORMAL );
}

/**
 * @ingroup WindowsPlatform
 * @brief Sleep this thread
 *
 * @param InSeconds		Seconds of sleep
 */
FORCEINLINE void Sys_Sleep( float InSeconds )
{
	Sleep( ( DWORD )( InSeconds * 1000.0 ) );
}

#endif // !WINDOWSTHREADING_INL