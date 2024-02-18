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

#ifndef MEMALLOC_BASE_H
#define MEMALLOC_BASE_H

#include "core/memory.h"

/**
 * @ingroup core
 * @brief Base class for the global memory allocator
 */
class CMemAllocBase : public IMemAlloc, public CUseSystemMemAllocForNew
{
public:
	/**
	 * @brief Try allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override
	{
		return Malloc( numBytes, alignment );
	}

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override
	{
		return Realloc( pOriginal, numBytes, alignment );
	}

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param pOriginal		Pointer to memory we are checking the size of
	* @param numBytes		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override
	{
		// Default implementation has no way of determining this
		return false;
	}

	/**
	* @brief Releases as much memory as possible. Must be called from the main thread
	* @param bTrimThreadCaches		Is need trim thread caches
	*/
	virtual void Trim( bool bTrimThreadCaches ) override
	{}

	/**
	 * @brief Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it
	 *
	 * @return Return TRUE if the allocator is guaranteed to be thread-safe, otherwise returns FALSE
	 */
	virtual bool IsInternallyThreadSafe() const
	{
		return false;
	}
};

#endif // !MEMALLOC_BASE_H