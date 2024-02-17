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

#ifndef WIN_THREADING_INL
#define WIN_THREADING_INL

/*
==================
Sys_InterlockedIncrement
==================
*/
FORCEINLINE int32 Sys_InterlockedIncrement( volatile int32* pValue )
{
	return ( int32 )InterlockedIncrement( ( LPLONG )pValue );
}

/*
==================
Sys_InterlockedDecrement
==================
*/
FORCEINLINE int32 Sys_InterlockedDecrement( volatile int32* pValue )
{
	return ( int32 )InterlockedDecrement( ( LPLONG )pValue );
}

/*
==================
Sys_InterlockedAdd
==================
*/
FORCEINLINE int32 Sys_InterlockedAdd( volatile int32* pValue, int32 amount )
{
	return ( int32 )InterlockedExchangeAdd( ( LPLONG )pValue, ( LONG )amount );
}

/*
==================
Sys_InterlockedExchange
==================
*/
FORCEINLINE int32 Sys_InterlockedExchange( volatile int32* pValue, int32 exchange )
{
	return ( int32 )InterlockedExchange( ( LPLONG )pValue, ( LONG )exchange );
}

/*
==================
Sys_InterlockedExchange64
==================
*/
FORCEINLINE int64 Sys_InterlockedExchange64( volatile int64* pValue, int64 exchange )
{
	return ( int64 )InterlockedExchange64( pValue, exchange );
}

/*
==================
Sys_InterlockedCompareExchange
==================
*/
FORCEINLINE int32 Sys_InterlockedCompareExchange( volatile int32* pDest, int32 exchange, int32 comperand )
{
	return ( int32 )InterlockedCompareExchange( ( LPLONG )pDest, ( LONG )exchange, ( LONG )comperand );
}

/*
==================
Sys_InterlockedCompareExchange64
==================
*/
FORCEINLINE int64 Sys_InterlockedCompareExchange64( volatile int64* pDest, int64 exchange, int64 comperand )
{
	return ( int64 )InterlockedCompareExchange64( pDest, exchange, comperand );
}

/*
==================
Sys_InterlockedCompareExchangePointer
==================
*/
FORCEINLINE void* Sys_InterlockedCompareExchangePointer( void** pDest, void* pExchange, void* pComperand )
{
	return InterlockedCompareExchangePointer( pDest, pExchange, pComperand );
}

/*
==================
Sys_InterlockedOr
==================
*/
FORCEINLINE int32 Sys_InterlockedOr( volatile int32* pDest, int32 value )
{
	return ( int32 )InterlockedOr( ( LPLONG )pDest, ( LONG )value );
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
FORCEINLINE void Sys_Sleep( float seconds )
{
	Sleep( ( DWORD )( seconds * 1000.0 ) );
}

#endif // !WIN_THREADING_INL