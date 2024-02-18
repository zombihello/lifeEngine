/**
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

#include "pch_core.h"
#include "core/memalloc_std.h"

/*
==================
CMemAllocStd::CMemAllocStd
==================
*/
CMemAllocStd::CMemAllocStd()
{
#if PLATFORM_WINDOWS
	// Enable low fragmentation heap http://msdn2.microsoft.com/en-US/library/aa366750.aspx
	intptr_t	pCRTHeapHandle	= _get_heap_handle();
	ULONG		enableLFH		= 2;
	HeapSetInformation( ( void* )pCRTHeapHandle, HeapCompatibilityInformation, &enableLFH, sizeof( enableLFH ) );
#endif // PLATFORM_WINDOWS
}

/*
==================
CMemAllocStd::Malloc
==================
*/
void* CMemAllocStd::Malloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	pResult = TryMalloc( numBytes, alignment );
	if ( !pResult && numBytes )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocStd::TryMalloc
==================
*/
void* CMemAllocStd::TryMalloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	alignment = Max<uint32>( numBytes >= 16 ? 16 : 8, alignment );
	void*	pResult = nullptr;

	// Allocate memory
#if PLATFORM_USE__ALIGNED_MALLOC
	pResult = _aligned_malloc( numBytes, alignment );
#else
	void*	pPtr = L_malloc_system( numBytes + alignment + sizeof( void* ) + sizeof( size_t ) );
	if ( pPtr )
	{
		pResult = Align( ( uint8* )pPtr + sizeof( void* ) + sizeof( size_t ), alignment );
		*( ( void** )( ( uint8* )pResult - sizeof( void* ) ) ) = pPtr;
		*( ( size_t* )( ( uint8* )pResult - sizeof( void* ) - sizeof( size_t ) ) ) = numBytes;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
	
	return pResult;
}

/*
==================
CMemAllocStd::Realloc
==================
*/
void* CMemAllocStd::Realloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	pResult = TryRealloc( pOriginal, numBytes, alignment );
	if ( !pResult && numBytes != 0 )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocStd::TryRealloc
==================
*/
void* CMemAllocStd::TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	alignment = Max<uint32>( numBytes >= 16 ? 16 : 8, alignment );
	void* pResult = nullptr;
	
#if PLATFORM_USE__ALIGNED_MALLOC
	if ( pOriginal && numBytes )
	{
		pResult = _aligned_realloc( pOriginal, numBytes, alignment );
	}
	else if ( !pOriginal )
	{
		pResult = _aligned_malloc( numBytes, alignment );
	}
	else
	{
		_aligned_free( pOriginal );
		pResult = nullptr;
	}
#else
	if ( pOriginal && numBytes )
	{
		// Can't use realloc as it might screw with alignment
		pResult = TryMalloc( numBytes, alignment );
		size_t	ptrSize = 0;
		bool	bResult = GetAllocationSize( pOriginal, ptrSize );
		Assert( ptrSize );
		L_memcpy( pResult, pOriginal, Min( numBytes, ptrSize ) );
		Free( pOriginal );
	}
	else if ( !pOriginal )
	{
		pResult = TryMalloc( numBytes, alignment );
	}
	else
	{
		L_free_system( *( ( void** )( ( uint8* )pOriginal - sizeof( void* ) ) ) );
		pResult = nullptr;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC

	return pResult;
}

/*
==================
CMemAllocStd::Free
==================
*/
void CMemAllocStd::Free( void* pOriginal )
{
#if PLATFORM_USE__ALIGNED_MALLOC
	_aligned_free( pOriginal );
#else
	if ( pOriginal )
	{
		L_free_system( *( ( void** )( ( uint8* )pOriginal - sizeof( void* ) ) ) );
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMemAllocStd::GetAllocationSize
==================
*/
bool CMemAllocStd::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	if ( !pOriginal )
	{
		return false;
	}

#if PLATFORM_USE__ALIGNED_MALLOC
	numBytes = _aligned_msize( pOriginal, 16, 0 ); // Assumes alignment of 16
	return true;
#else
	numBytes = *( ( size_t* )( ( uint8* )pOriginal - sizeof( void* ) - sizeof( size_t ) ) );
	return true;
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMemAllocStd::IsInternallyThreadSafe
==================
*/
bool CMemAllocStd::IsInternallyThreadSafe() const
{
#if PLATFORM_IS_ANSI_MALLOC_THREADSAFE
	return true;
#else
	return false;
#endif
}