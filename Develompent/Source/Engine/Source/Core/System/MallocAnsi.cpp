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

#include "Core/System/MallocAnsi.h"
#include "Core/Core.h"

/*
==================
CMallocAnsi::CMallocAnsi
==================
*/
CMallocAnsi::CMallocAnsi()
{
#if PLATFORM_WINDOWS
	// Enable low fragmentation heap http://msdn2.microsoft.com/en-US/library/aa366750.aspx
	intptr_t	crtHeapHandle	= _get_heap_handle();
	ULONG		enableLFH		= 2;
	HeapSetInformation( ( void* )crtHeapHandle, HeapCompatibilityInformation, &enableLFH, sizeof( enableLFH ) );
#endif // PLATFORM_WINDOWS
}

/*
==================
CMallocAnsi::Malloc
==================
*/
void* CMallocAnsi::Malloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryMalloc( InSize, InAlignment );
	if ( !result && InSize )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocAnsi::TryMalloc
==================
*/
void* CMallocAnsi::TryMalloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
	void*		result = nullptr;

	// Allocate memory
#if PLATFORM_USE__ALIGNED_MALLOC
	result	= _aligned_malloc( InSize, InAlignment );
#else
	void*	ptr = malloc( InSize + InAlignment + sizeof( void* ) + sizeof( size_t ) );
	if ( ptr )
	{
		result = Align( ( uint8* )ptr + sizeof( void* ) + sizeof( size_t ), InAlignment );
		*( ( void** )( ( uint8* )result - sizeof( void* ) ) ) = ptr;
		*( ( size_t* )( ( uint8* )result - sizeof( void* ) - sizeof( size_t ) ) ) = InSize;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
	
	return result;
}

/*
==================
CMallocAnsi::Realloc
==================
*/
void* CMallocAnsi::Realloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryRealloc( InOriginal, InSize, InAlignment );
	if ( !result && InSize != 0 )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocAnsi::TryRealloc
==================
*/
void* CMallocAnsi::TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
	void*		result = nullptr;
	
#if PLATFORM_USE__ALIGNED_MALLOC
	if ( InOriginal && InSize )
	{
		result = _aligned_realloc( InOriginal, InSize, InAlignment );
	}
	else if ( !InOriginal )
	{
		result = _aligned_malloc( InSize, InAlignment );
	}
	else
	{
		_aligned_free( InOriginal );
		result = nullptr;
	}
#else
	if ( InOriginal && InSize )
	{
		// Can't use realloc as it might screw with alignment
		result = TryMalloc( InSize, InAlignment );
		size_t	ptrSize = GetAllocationSize( InOriginal );
		Memory::Memcpy( result, InOriginal, Min( InSize, ptrSize ) );
		Free( InOriginal );
	}
	else if ( !InOriginal )
	{
		result = TryMalloc( InSize, InAlignment );
	}
	else
	{
		free( *( ( void** )( ( uint8* )InOriginal - sizeof( void* ) ) ) );
		result = nullptr;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC

	return result;
}

/*
==================
CMallocAnsi::Free
==================
*/
void CMallocAnsi::Free( void* InOriginal )
{
#if PLATFORM_USE__ALIGNED_MALLOC
	_aligned_free( InOriginal );
#else
	if ( InOriginal )
	{
		free( *( ( void** )( ( uint8* )InOriginal - sizeof( void* ) ) ) );
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMallocAnsi::GetAllocationSize
==================
*/
bool CMallocAnsi::GetAllocationSize( void* InOriginal, size_t& OutSize )
{
	if ( !InOriginal )
	{
		return false;
	}

#if PLATFORM_USE__ALIGNED_MALLOC
	OutSize = _aligned_msize( InOriginal, 16, 0 ); // Assumes alignment of 16
	return true;
#else
	OutSize = *( ( size_t* )( ( uint8* )InOriginal - sizeof( void* ) - sizeof( size_t ) ) );
	return true;
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMallocAnsi::IsInternallyThreadSafe
==================
*/
bool CMallocAnsi::IsInternallyThreadSafe() const
{
#if PLATFORM_IS_ANSI_MALLOC_THREADSAFE
	return true;
#else
	return false;
#endif
}