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

#include "Core/System/MallocMimalloc.h"
#include "Core/Core.h"

#if PLATFORM_SUPPORTS_MIMALLOC
#include <mimalloc.h>

/*
==================
CMallocMimalloc::Malloc
==================
*/
void* CMallocMimalloc::Malloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
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
CMallocMimalloc::TryMalloc
==================
*/
void* CMallocMimalloc::TryMalloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	newPtr = nullptr;
	if ( InAlignment != DEFAULT_ALIGNMENT )
	{
		newPtr = mi_malloc_aligned( InSize, Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment ) );
	}
	else
	{
		newPtr = mi_malloc_aligned( InSize, InSize >= 16 ? 16 : 8 );
	}
	return newPtr;
}

/*
==================
CMallocMimalloc::Realloc
==================
*/
void* CMallocMimalloc::Realloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryRealloc( InOriginal, InSize, InAlignment );
	if ( !result && InSize )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocMimalloc::TryRealloc
==================
*/
void* CMallocMimalloc::TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	newPtr = nullptr;
	if ( InSize == 0 )
	{
		mi_free( InOriginal );
		return nullptr;
	}

	if ( InAlignment != DEFAULT_ALIGNMENT )
	{
		newPtr = mi_realloc_aligned( InOriginal, InSize, Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment ) );
	}
	else
	{
		newPtr = mi_realloc( InOriginal, InSize );
	}

	return newPtr;
}

/*
==================
CMallocMimalloc::Free
==================
*/
void CMallocMimalloc::Free( void* InOriginal )
{
	if ( !InOriginal )
	{
		return;
	}

	mi_free( InOriginal );
}

/*
==================
CMallocMimalloc::GetAllocationSize
==================
*/
bool CMallocMimalloc::GetAllocationSize( void* InOriginal, size_t& OutSize )
{
	OutSize = mi_malloc_size( InOriginal );
	return true;
}

/*
==================
CMallocMimalloc::Trim
==================
*/
void CMallocMimalloc::Trim( bool InIsTrimThreadCaches )
{
	mi_collect( InIsTrimThreadCaches );
}
#endif // PLATFORM_SUPPORTS_MIMALLOC