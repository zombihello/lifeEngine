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

#ifndef WINDOWSMEMORY_H
#define WINDOWSMEMORY_H

#include "Core/System/GenericPlatformMemory.h"

/**
 * @ingroup WindowsPlatform
 * @brief Windows implementation of the memory OS functions
 */
struct WindowsPlatformMemory : public GenericPlatformMemory
{
	/**
	 * @brief Allocates InSize bytes of uninitialized storage
	 *
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	static FORCEINLINE void* Malloc( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		void*	result = TryMalloc( InSize, InAlignment );
		if ( !result && InSize )
		{
			OutOfMemory( InSize, InAlignment );
			return nullptr;
		}

		return result;
	}

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param InOriginal	Pointer to the memory area to be reallocated
	 * @param InSize		New size of the array
	 * @param InAlignment	Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer InOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer InOriginal remains valid and may need to be deallocated with Free
	 */
	static FORCEINLINE void* Realloc( void* InOriginal, size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		void* result = TryRealloc( InOriginal, InSize, InAlignment );
		if ( !result && InSize )
		{
			OutOfMemory( InSize, InAlignment );
			return nullptr;
		}

		return result;
	}

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param InOriginal	Pointer to the memory to deallocate
	 */
	static FORCEINLINE void Free( void* InOriginal )
	{
		_aligned_free( InOriginal );
	}

private:
	/**
	 * @brief Try allocates InSize bytes of uninitialized storage
	 * 
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	static FORCEINLINE void* TryMalloc( size_t InSize, uint32 InAlignment )
	{
		void*	result = nullptr;
		if ( InAlignment != DEFAULT_ALIGNMENT )
		{
			InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
			result = _aligned_malloc( InSize, InAlignment );
		}
		else
		{
			result = _aligned_malloc( InSize, InSize >= 16 ? 16 : 8 );
		}

		return result;
	}

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param InOriginal	Pointer to the memory area to be reallocated
	 * @param InSize		New size of the array
	 * @param InAlignment	Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer InOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer InOriginal remains valid and may need to be deallocated with Free
	 */
	static FORCEINLINE void* TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment )
	{
		void*	result = nullptr;
		if ( InAlignment != DEFAULT_ALIGNMENT )
		{
			InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
			result = _aligned_realloc( InOriginal, InSize, InAlignment );
		}
		else
		{
			result = _aligned_realloc( InOriginal, InSize, InSize >= 16 ? 16 : 8 );
		}

		return result;
	}
};

/**
 * @ingroup WindowsPlatform
 * @brief Typedef of Windows platform memory
 */
typedef WindowsPlatformMemory		Memory;

#endif // !WINDOWSMEMORY_H