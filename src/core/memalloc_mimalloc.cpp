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
#include "core/memalloc_mimalloc.h"

#if PLATFORM_SUPPORTS_MIMALLOC
#include <mimalloc.h>

/*
==================
CMemAllocMimalloc::Malloc
==================
*/
void* CMemAllocMimalloc::Malloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
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
CMemAllocMimalloc::TryMalloc
==================
*/
void* CMemAllocMimalloc::TryMalloc( size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	pNewPtr = nullptr;
	if ( alignment != DEFAULT_ALIGNMENT )
	{
		pNewPtr = mi_malloc_aligned( numBytes, Max<uint32>( numBytes >= 16 ? 16 : 8, alignment ) );
	}
	else
	{
		pNewPtr = mi_malloc_aligned( numBytes, numBytes >= 16 ? 16 : 8 );
	}
	return pNewPtr;
}

/*
==================
CMemAllocMimalloc::Realloc
==================
*/
void* CMemAllocMimalloc::Realloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	pResult = TryRealloc( pOriginal, numBytes, alignment );
	if ( !pResult && numBytes )
	{
		Sys_OutOfMemory( numBytes, alignment );
	}

	return pResult;
}

/*
==================
CMemAllocMimalloc::TryRealloc
==================
*/
void* CMemAllocMimalloc::TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	pNewPtr = nullptr;
	if ( numBytes == 0 )
	{
		mi_free( pOriginal );
		return nullptr;
	}

	if ( alignment != DEFAULT_ALIGNMENT )
	{
		pNewPtr = mi_realloc_aligned( pOriginal, numBytes, Max<uint32>( numBytes >= 16 ? 16 : 8, alignment ) );
	}
	else
	{
		pNewPtr = mi_realloc( pOriginal, numBytes );
	}

	return pNewPtr;
}

/*
==================
CMemAllocMimalloc::Free
==================
*/
void CMemAllocMimalloc::Free( void* pOriginal )
{
	if ( !pOriginal )
	{
		return;
	}

	mi_free( pOriginal );
}

/*
==================
CMemAllocMimalloc::GetAllocationSize
==================
*/
bool CMemAllocMimalloc::GetAllocationSize( void* pOriginal, size_t& numBytes )
{
	numBytes = mi_malloc_size( pOriginal );
	return true;
}

/*
==================
CMemAllocMimalloc::Trim
==================
*/
void CMemAllocMimalloc::Trim( bool bTrimThreadCaches )
{
	mi_collect( bTrimThreadCaches );
}

/*
==================
CMemAllocMimalloc::IsInternallyThreadSafe
==================
*/
bool CMemAllocMimalloc::IsInternallyThreadSafe() const
{
	return true;
}
#endif // PLATFORM_SUPPORTS_MIMALLOC