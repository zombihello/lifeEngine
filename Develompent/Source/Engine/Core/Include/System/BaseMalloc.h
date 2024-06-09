/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEMALLOC_H
#define BASEMALLOC_H

#include "Misc/Platform.h"

enum
{
	DEFAULT_ALIGNMENT	= 0,		/**< Default allocator alignment. If the default is specified, the allocator applies to engine rules. Blocks >= 16 bytes will be 16-byte-aligned, Blocks < 16 will be 8-byte aligned. If the allocator does not support allocation alignment, the alignment will be ignored */
	MIN_ALIGNMENT		= 8			/**< Minimum allocator alignment */
};

/**
 * @ingroup Core
 * @brief Inherit from CUseSystemMallocForNew if you want your objects to be placed in memory
 * allocated by the system malloc routines, bypassing g_Malloc. This is e.g. used by CBaseMalloc
 * itself
 */
class CUseSystemMallocForNew
{
public:
	/**
	 * @brief Overloaded new operator using the system allocator
	 *
	 * @param InSize	Amount of memory to allocate (in bytes)
	 * @return Return a pointer to a block of memory with size InSize or NULL
	 */
	void* operator new( size_t InSize );

	/**
	 * @brief Overloaded delete operator using the system allocator
	 *
	 * @param InPtr		Pointer to delete
	 */
	void operator delete( void* InPtr );

	/**
	 * @brief Overloaded array new operator using the system allocator.
	 *
	 * @param InSize	Amount of memory to allocate (in bytes)
	 * @return Return a pointer to a block of memory with size InSize or NULL
	 */
	void* operator new[]( size_t InSize );

	/**
	 * @brief Overloaded array delete operator using the system allocator
	 *
	 * @param InPtr		Pointer to delete
	 */
	void operator delete[]( void* InPtr );
};

/**
 * @ingroup Core
 * @brief The global memory allocator's interface
 */
class CBaseMalloc : public CUseSystemMallocForNew
{
public:
	/**
	 * @brief Allocates InSize bytes of uninitialized storage
	 *
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* Malloc( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Try allocates InSize bytes of uninitialized storage
	 *
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* TryMalloc( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		return Malloc( InSize, InAlignment );
	}

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param InOriginal	Pointer to the memory area to be reallocated
	 * @param InSize		New size of the array
	 * @param InAlignment	Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer InOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer InOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* Realloc( void* InOriginal, size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT ) = 0;

	/**
	 * @brief Try reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param InOriginal	Pointer to the memory area to be reallocated
	 * @param InSize		New size of the array
	 * @param InAlignment	Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer InOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer InOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT )
	{
		return Realloc( InOriginal, InSize, InAlignment );
	}

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param InOriginal	Pointer to the memory to deallocate
	 */
	virtual void Free( void* InOriginal ) = 0;

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param InOriginal		Pointer to memory we are checking the size of
	* @param OutSize		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* InOriginal, size_t& OutSize )
	{
		// Default implementation has no way of determining this
		return false; 
	}

	/**
	* @brief Releases as much memory as possible. Must be called from the main thread
	* @param InIsTrimThreadCaches	Is need trim thread caches
	*/
	virtual void Trim( bool InIsTrimThreadCaches )
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

	/**
	 * @brief Gets descriptive name for logging purposes
	 * @return Return pointer to human-readable malloc name
	 */
	virtual const tchar* GetDescriptiveName() const
	{
		return TEXT( "Unspecified allocator" );
	}
};

#endif // !BASEMALLOC_H