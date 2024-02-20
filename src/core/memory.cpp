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
#include "core/memalloc_mimalloc.h"
#include "core/memalloc_threadsafeproxy.h"

/*
==================
AllocateMemAlloc
==================
*/
static CMemAllocBase* AllocateMemAlloc()
{
#if PLATFORM_SUPPORTS_MIMALLOC
	// Mimalloc default allocator because it has great performance
	return new CMemAllocMimalloc();
#endif // PLATFORM_SUPPORTS_MIMALLOC

	// Fallback allocator
	return new CMemAllocStd();
}

/*
==================
MemAlloc
==================
*/
IMemAlloc* MemAlloc()
{
	static IMemAlloc*	s_pMemAlloc = nullptr;
	
	// If the global memory allocator not created yet then do it now!
	if ( !s_pMemAlloc )
	{
		// Allocate a new memory allocator
		CMemAllocBase*	pMemAlloc = AllocateMemAlloc();

		// If the allocator is already thread safe, there is no need for the thread safe proxy
		if ( !pMemAlloc->IsInternallyThreadSafe() )
		{
			pMemAlloc = new CMemAllocThreadSafeProxy( pMemAlloc );
		}

		// Remember our the global memory allocator
		s_pMemAlloc = pMemAlloc;
	}

	return s_pMemAlloc;
}

/*
==================
Sys_OutOfMemory
==================
*/
void Sys_OutOfMemory( size_t numBytes, uint32 alignment )
{
	Sys_Error( "Ran out of memory allocating %llu bytes with alignment %u", numBytes, alignment );
}