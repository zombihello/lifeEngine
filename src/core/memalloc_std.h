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

#ifndef MEMALLOC_STD_H
#define MEMALLOC_STD_H

#include "core/memalloc_base.h"

/**
 * @ingroup core
 * @brief Std C memory allocator
 */
class CMemAllocStd : public CMemAllocBase
{
public:
	/**
	 * @brief Constructor enabling low fragmentation heap on platforms supporting it
	 */
	CMemAllocStd();

	/**
	 * @brief Allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* Malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Try allocates numBytes bytes of uninitialized storage
	 *
	 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
	 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* TryMalloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* Realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param pOriginal		Pointer to the memory area to be reallocated
	 * @param numBytes		New size of the array
	 * @param alignment		Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* TryRealloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT ) override;

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param pOriginal		Pointer to the memory to deallocate
	 */
	virtual void Free( void* pOriginal ) override;

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param pOriginal		Pointer to memory we are checking the size of
	* @param numBytes		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* pOriginal, size_t& numBytes ) override;

	/**
	 * @brief Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it
	 * 
	 * @return Return TRUE if the allocator is guaranteed to be thread-safe, otherwise returns FALSE
	 */
	virtual bool IsInternallyThreadSafe() const override;
};

#endif // !MEMALLOC_STD_H