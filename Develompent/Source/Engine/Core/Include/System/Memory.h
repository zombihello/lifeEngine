/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <cstdlib>

#include "Misc/Platform.h"
#include "System/BaseMalloc.h"

/**
 * @ingroup Core
 * @brief The global memory allocator
 */
extern CBaseMalloc*		g_Malloc;

// Platform specific memory implementation
#if PLATFORM_WINDOWS
	#include "WindowsMemory.h"
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS

/**
 * @ingroup Core
 * @brief Memory
 */
struct Memory
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
		return PlatformMemory::Memmove( InDest, InSrc, InSize );
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
		return PlatformMemory::Memcmp( InBuf1, InBuf2, InSize );
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
		return PlatformMemory::Memset( InDest, InChar, InSize );
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
		return PlatformMemory::Memzero( InDest, InSize );
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
		return PlatformMemory::Memcpy( InDest, InSrc, InSize );
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
		if ( !g_Malloc )
		{
			CreateGlobalMalloc();
		}
		return g_Malloc->Malloc( InSize, InAlignment );
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
		if ( !g_Malloc )
		{
			CreateGlobalMalloc();
		}
		return g_Malloc->Realloc( InOriginal, InSize, InAlignment );
	}

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param InOriginal	Pointer to the memory to deallocate
	 */
	static FORCEINLINE void Free( void* InOriginal )
	{
		if ( !g_Malloc )
		{
			CreateGlobalMalloc();
		}
		return g_Malloc->Free( InOriginal );
	}

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param InOriginal		Pointer to memory we are checking the size of
	* @return If possible, returns the size of the passed in pointer, otherwise return 0
	*/
	static FORCEINLINE size_t GetAllocationSize( void* InOriginal )
	{
		if ( !g_Malloc )
		{
			CreateGlobalMalloc();
		}

		size_t	size = 0;
		return g_Malloc->GetAllocationSize( InOriginal, size ) ? size : 0;
	}

	/**
	 * @brief C style memory allocation that fall back to C runtime
	 *
	 * @param InSize		Number of bytes to allocate
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with SystemFree(). On failure, returns a NULL pointer
	 */
	static FORCEINLINE void* SystemMalloc( size_t InSize )
	{
		return ::malloc( InSize );
	}

	/**
	 * @brief C style memory deallocation that fall back to C runtime
	 * @param InOriginal	Pointer to the memory to deallocate
	 */
	static FORCEINLINE void SystemFree( void* InPtr )
	{
		::free( InPtr );
	}

private:
	/**
	 * @brief Create global malloc
	 * This function allocate g_Malloc and must be called one time
	 */
	static void CreateGlobalMalloc();
};

// Override default memory allocation
#include "System/MemoryOverride.h"

#endif // !MEMORY_H