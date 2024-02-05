/**
 * @file
 * @addtogroup Core Core
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

#ifndef GENERICPLATFORMMEMORY_H
#define GENERICPLATFORMMEMORY_H

/**
 * @ingroup Core
 * @brief Generic implementation for most platforms
 */
struct GenericPlatformMemory
{
	/**
	 * @brief Copies count characters from the object pointed to by InSrc to the object pointed to by InDest
	 * 
	 * @param InDest	Pointer to the memory location to copy to
	 * @param InSrc		Pointer to the memory location to copy from
	 * @param InSize	Number of bytes to copy
	 * @return Returns pointer to the memory location to copy to (InDest)
	 */
	static FORCEINLINE void* Memmove( void* InDest, const void* InSrc, size_t InSize )
	{
		return memmove( InDest, InSrc, InSize );
	}

	/**
	 * @brief Compares the first count bytes of InBuf1 and InBuf2 arrays
	 * 
	 * @param InBuf1	Pointer to the memory buffers to compare
	 * @param InBuf2	Pointer to the memory buffers to compare
	 * @param InSize	Number of bytes to examine
	 * @return Return negative value if the first differing byte in InBuf1 is less than the corresponding byte in InBuf2. 0 if all InSize bytes of InBuf1 and InBuf2 are equal. Positive value if the first differing byte in InBuf1 is greater than the corresponding byte in InBuf2.
	 */
	static FORCEINLINE int32 Memcmp( const void* InBuf1, const void* InBuf2, size_t InSize )
	{
		return memcmp( InBuf1, InBuf2, InSize );
	}

	/**
	 * @brief Copies the value InChar into each of the first InSize characters of the object pointed to by InDest
	 * 
	 * @param InDest	Pointer to the object to fill
	 * @param InChar	Fill byte
	 * @param InSize	Number of bytes to fill
	 * @return Return pointer to the object to fill (InDest)
	 */
	static FORCEINLINE void* Memset( void* InDest, uint8 InChar, size_t InSize )
	{
		return memset( InDest, InChar, InSize );
	}

	/**
	 * @brief Fills each of the first InSize characters by zero pointed to by InDest
	 * 
	 * @param InDest	Pointer to the object to fill
	 * @param InSize	Number of bytes to fill
	 * @return Return pointer to the object to fill (InDest)
	 */
	static FORCEINLINE void* Memzero( void* InDest, size_t InSize )
	{
		return memset( InDest, 0, InSize );
	}

	/**
	 * @brief Copies InSize bytes from the object pointed to by InSrc to the object pointed to by InDest
	 * 
	 * @param InDest	Pointer to the memory location to copy to
	 * @param InSrc		Pointer to the memory location to copy from
	 * @param InSize	Number of bytes to copy
	 * @return Return pointer to the memory location to copy to (InDest)
	 */
	static FORCEINLINE void* Memcpy( void* InDest, const void* InSrc, size_t InSize )
	{
		return memcpy( InDest, InSrc, InSize );
	}

	/**
	 * @brief Allocates InSize bytes of uninitialized storage
	 * 
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	static FORCEINLINE void* Malloc( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		void*	result = malloc( InSize );
		if ( !result && InSize )
		{
			OutOfMemory( InSize, InAlignment );
			return nullptr;
		}

		return result;
	}

	/**
	 * @brief Allocates InSize bytes of uninitialized storage filed by zero
	 * 
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	static FORCEINLINE void* MallocZeroed( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		void*	data = Malloc( InSize, InAlignment );
		Memzero( data, InSize );
		return data;
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
		void*	result = realloc( InOriginal, InSize );
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
		free( InOriginal );
	}

protected:
	/**
	 * @brief Handle out of memory
	 * 
	 * @param InSize		OOM allocation size
	 * @param InAlignment	OOM	allocation alignment
	 */
	static void OutOfMemory( size_t InSize, uint32 InAlignment );
};

#endif // !GENERICPLATFORMMEMORY_H