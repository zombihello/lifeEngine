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

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdlib>

#include "core/coreapi.h"
#include "core/platform.h"
#include "core/imemalloc.h"

/**
 * @ingroup core
 * @brief Copies count characters from the object pointed to by pSrc to the object pointed to by pDest
 * 
 * @param pDest		Pointer to the memory location to copy to
 * @param pSrc		Pointer to the memory location to copy from
 * @param numBytes	Number of bytes to copy
 * @return Returns pointer to the memory location to copy to (pDest)
 */
FORCEINLINE void* L_memmove( void* pDest, const void* pSrc, size_t numBytes );

/**
 * @ingroup core
 * @brief Compares the first count bytes of pBuf1 and pBuf2 arrays
 * 
 * @param pBuf1		Pointer to the memory buffers to compare
 * @param pBuf2		Pointer to the memory buffers to compare
 * @param numBytes	Number of bytes to examine
 * @return Return negative value if the first differing byte in pBuf1 is less than the corresponding byte in pBuf2. 0 if all numBytes bytes of pBuf1 and pBuf2 are equal. Positive value if the first differing byte in pBuf1 is greater than the corresponding byte in pBuf2
 */
FORCEINLINE int32 L_memcmp( const void* pBuf1, const void* pBuf2, size_t numBytes );

/**
 * @ingroup core
 * @brief Copies the value c into each of the first numBytes characters of the object pointed to by pDest
 * 
 * @param pDest		Pointer to the object to fill
 * @param c			Fill byte
 * @param numBytes	Number of bytes to fill
 * @return Return pointer to the object to fill (pDest)
 */
FORCEINLINE void* L_memset( void* pDest, uint8 c, size_t numBytes );

/**
 * @ingroup core 
 * @brief Fills each of the first numBytes characters by zero pointed to by pDest
 * 
 * @param pDest		Pointer to the object to fill
 * @param numBytes	Number of bytes to fill
 * @return Return pointer to the object to fill (pDest)
 */
FORCEINLINE void* L_memzero( void* pDest, size_t numBytes );

/**
 * @ingroup core 
 * @brief Copies numBytes bytes from the object pointed to by pSrc to the object pointed to by pDest
 * 
 * @param pDest		Pointer to the memory location to copy to
 * @param pSrc		Pointer to the memory location to copy from
 * @param numBytes	Number of bytes to copy
 * @return Return pointer to the memory location to copy to (pDest)
 */
FORCEINLINE void* L_memcpy( void* pDest, const void* pSrc, size_t numBytes );

/**
 * @ingroup core 
 * @brief Allocates numBytes bytes of uninitialized storage
 * 
 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with L_free() or L_realloc(). On failure, returns a NULL pointer
 */
FORCEINLINE void* L_malloc( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )
{
	return g_pMemAlloc->Malloc( numBytes, alignment );
}

/**
 * @ingroup core 
 * @brief Allocates numBytes bytes of uninitialized storage filed by zero
 * 
 * @param numBytes		Number of bytes to allocate. An integral multiple of alignment
 * @param alignment		Specifies the alignment. Must be a valid alignment supported by the implementation
 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with L_free() or L_realloc(). On failure, returns a NULL pointer
 */
FORCEINLINE void* L_malloc_zeroed( size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )
{
	void*	pData = L_malloc( numBytes, alignment );
	L_memzero( pData, numBytes );
	return pData;
}

/**
 * @ingroup core 
 * @brief Reallocates the given area of memory. It must be previously allocated by L_malloc or L_malloc_zeroed and not yet freed with L_free, otherwise, the results are undefined
 * 
 * @param pOriginal		Pointer to the memory area to be reallocated
 * @param numBytes		New size of the array
 * @param alignment		Alignment
 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with L_free or L_realloc. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with L_free
 */
FORCEINLINE void* L_realloc( void* pOriginal, size_t numBytes, uint32 alignment = DEFAULT_ALIGNMENT )
{
	return g_pMemAlloc->Realloc( pOriginal, numBytes, alignment );
}

/**
 * @ingroup core 
 * @brief Deallocates the space previously allocated by L_malloc or L_realloc
 * @param pOriginal		Pointer to the memory to deallocate
 */
FORCEINLINE void L_free( void* pOriginal )
{
	return g_pMemAlloc->Free( pOriginal );
}

/**
* @ingroup core 
* @brief If possible determine the size of the memory allocated at the given address
*
* @param InOriginal		Pointer to memory we are checking the size of
* @return If possible, returns the size of the passed in pointer, otherwise return 0
*/
FORCEINLINE size_t L_allocsize( void* pOriginal )
{
	size_t	numBytes = 0;
	return g_pMemAlloc->GetAllocationSize( pOriginal, numBytes ) ? numBytes : 0;
}

/**
 * @ingroup core 
 * @brief C style memory allocation that fall back to C runtime
 *
 * @param numBytes		Number of bytes to allocate
 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with L_free_system() or L_realloc_system(). On failure, returns a NULL pointer
 */
FORCEINLINE void* L_malloc_system( size_t numBytes )
{
	return malloc( numBytes );
}

/**
 * @ingroup core
 * @brief Reallocates the given area of memory. It must be previously allocated by L_malloc_system and not yet freed with L_free_system, otherwise, the results are undefined
 *
 * @param pOriginal		Pointer to the memory area to be reallocated
 * @param numBytes		New size of the array
 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with L_free_system or L_realloc_system. The original pointer pOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer pOriginal remains valid and may need to be deallocated with L_free_system
 */
FORCEINLINE void* L_realloc_system( void* pOriginal, size_t numBytes )
{
	return realloc( pOriginal, numBytes );
}

/**
 * @ingroup core 
 * @brief C style memory deallocation that fall back to C runtime
 * @param pOriginal		Pointer to the memory to deallocate
 */
FORCEINLINE void L_free_system( void* pOriginal )
{
	free( pOriginal );
}

/**
 * @ingroup core
 * @brief Inherit from CUseSystemMemAllocForNew if you want your objects to be placed in memory
 * allocated by the system memory allocator routines, bypassing engine's global the one
 */
class CUseSystemMemAllocForNew
{
public:
	/**
	 * @brief Overloaded new operator using the system allocator
	 *
	 * @param numBytes	Amount of memory to allocate (in bytes)
	 * @return Return a pointer to a block of memory with size numBytes or NULL
	 */
	void* operator new( size_t numBytes )
	{
		return L_malloc_system( numBytes );
	}

	/**
	 * @brief Overloaded delete operator using the system allocator
	 *
	 * @param pPtr		Pointer to delete
	 */
	void operator delete( void* pPtr )
	{
		L_free_system( pPtr );
	}

	/**
	 * @brief Overloaded array new operator using the system allocator.
	 *
	 * @param numBytes	Amount of memory to allocate (in bytes)
	 * @return Return a pointer to a block of memory with size numBytes or NULL
	 */
	void* operator new[]( size_t numBytes )
	{
		return L_malloc_system( numBytes );
	}

	/**
	 * @brief Overloaded array delete operator using the system allocator
	 *
	 * @param pPtr		Pointer to delete
	 */
	void operator delete[]( void* pPtr )
	{
		L_free_system( pPtr );
	}
};

// Include implementation of platform specific inline functions
#if PLATFORM_WINDOWS
	#include "core/platforms/windows/win_memory.inl"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

// Override the global memory allocator
#include "core/memoverride.h"

#endif // !MEMORY_H