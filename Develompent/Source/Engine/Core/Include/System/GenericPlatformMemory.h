/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
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
	 * @brief Enumeration current is being used allocator
	 */
	enum EMemoryAllocatorToUse
	{
		MAU_Std,		/**< Default C allocator */
		MAU_Mimalloc,	/**< Mimalloc */
		MAU_Platform	/**< Custom platform specific allocator */ 
	};

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
	 * @brief Handle out of memory
	 * 
	 * @param InSize		OOM allocation size
	 * @param InAlignment	OOM	allocation alignment
	 */
	static void OutOfMemory( size_t InSize, uint32 InAlignment );

	/**
	 * @brief Allocate the default allocator for current platform
	 * @return Return allocated the default allocator for current platform
	 */
	static CBaseMalloc* AllocDefaultAllocator();

	static EMemoryAllocatorToUse	allocatorToUse;	/**< Current allocator */
};

#endif // !GENERICPLATFORMMEMORY_H