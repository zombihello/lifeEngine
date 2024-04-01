/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSTHREADING_INL
#define WINDOWSTHREADING_INL

/*
==================
Sys_InterlockedIncrement
==================
*/
FORCEINLINE int32 Sys_InterlockedIncrement( volatile int32* InValue )
{
	return ( int32 )InterlockedIncrement( ( LPLONG )InValue );
}

/*
==================
Sys_InterlockedDecrement
==================
*/
FORCEINLINE int32 Sys_InterlockedDecrement( volatile int32* InValue )
{
	return ( int32 )InterlockedDecrement( ( LPLONG )InValue );
}

/*
==================
Sys_InterlockedAdd
==================
*/
FORCEINLINE int32 Sys_InterlockedAdd( volatile int32* InValue, int32 InAmount )
{
	return ( int32 )InterlockedExchangeAdd( ( LPLONG )InValue, ( LONG )InAmount );
}

/*
==================
Sys_InterlockedExchange
==================
*/
FORCEINLINE int32 Sys_InterlockedExchange( volatile int32* InValue, int32 InExchange )
{
	return ( int32 )InterlockedExchange( ( LPLONG )InValue, ( LONG )InExchange );
}

/*
==================
Sys_InterlockedExchange64
==================
*/
FORCEINLINE int64 Sys_InterlockedExchange64( volatile int64* InValue, int64 InExchange )
{
	return ( int64 )InterlockedExchange64( InValue, InExchange );
}

/*
==================
Sys_InterlockedCompareExchange
==================
*/
FORCEINLINE int32 Sys_InterlockedCompareExchange( volatile int32* InDest, int32 InExchange, int32 InComperand )
{
	return ( int32 )InterlockedCompareExchange( ( LPLONG )InDest, ( LONG )InExchange, ( LONG )InComperand );
}

/*
==================
Sys_InterlockedCompareExchange64
==================
*/
FORCEINLINE int64 Sys_InterlockedCompareExchange64( volatile int64* InDest, int64 InExchange, int64 InComperand )
{
	return ( int64 )InterlockedCompareExchange64( InDest, InExchange, InComperand );
}

/*
==================
Sys_InterlockedCompareExchangePointer
==================
*/
FORCEINLINE void* Sys_InterlockedCompareExchangePointer( void** InDest, void* InExchange, void* InComperand )
{
	return InterlockedCompareExchangePointer( InDest, InExchange, InComperand );
}

/*
==================
Sys_InterlockedOr
==================
*/
FORCEINLINE int32 Sys_InterlockedOr( volatile int32* InDest, int32 InValue )
{
	return ( int32 )InterlockedOr( ( LPLONG )InDest, ( LONG )InValue );
}

/*
==================
Sys_GetCurrentThreadId
==================
*/
FORCEINLINE uint32 Sys_GetCurrentThreadId()
{
	return GetCurrentThreadId();
}

/*
==================
Sys_Yield
==================
*/
FORCEINLINE void Sys_Yield()
{
	Sleep( 0 );
}

/*
==================
Sys_Sleep
==================
*/
FORCEINLINE void Sys_Sleep( float InSeconds )
{
	Sleep( ( DWORD )( InSeconds * 1000.0 ) );
}

#endif // !WINDOWSTHREADING_INL