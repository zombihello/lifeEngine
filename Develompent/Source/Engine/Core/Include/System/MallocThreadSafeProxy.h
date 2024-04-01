/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MALLOCTHREADSAFEPROXY_H
#define MALLOCTHREADSAFEPROXY_H

#include "System/BaseMalloc.h"
#include "System/Threading.h"

/**
 * @ingroup Core
 * @brief CMalloc proxy that synchronizes access, making the used malloc thread safe
 */
class CMallocThreadSafeProxy : public CBaseMalloc
{
public:
	/**
	 * @brief Constructor
	 * @param InMalloc	CMalloc that is going to be used for actual allocations
	 */
	CMallocThreadSafeProxy( CBaseMalloc* InMalloc )
		: usedMalloc( InMalloc )
	{}

	/**
	 * @brief Allocates InSize bytes of uninitialized storage
	 *
	 * @param InSize		Number of bytes to allocate. An integral multiple of InAlignment
	 * @param InAlignment	Specifies the alignment. Must be a valid alignment supported by the implementation
	 * @return On success, returns the pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free() or Realloc(). On failure, returns a NULL pointer
	 */
	virtual void* Malloc( size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT ) override
	{
		CScopeLock		scopeLock( &synchronizationObject );
		return usedMalloc->Malloc( InSize, InAlignment );
	}

	/**
	 * @brief Reallocates the given area of memory. It must be previously allocated by Malloc or MallocZeroed and not yet freed with Free, otherwise, the results are undefined
	 *
	 * @param InOriginal	Pointer to the memory area to be reallocated
	 * @param InSize		New size of the array
	 * @param InAlignment	Alignment
	 * @return On success, returns a pointer to the beginning of newly allocated memory. To avoid a memory leak, the returned pointer must be deallocated with Free or Realloc. The original pointer InOriginal is invalidated and any access to it is undefined behavior (even if reallocation was in-place). On failure, returns a null pointer. The original pointer InOriginal remains valid and may need to be deallocated with Free
	 */
	virtual void* Realloc( void* InOriginal, size_t InSize, uint32 InAlignment = DEFAULT_ALIGNMENT ) override
	{
		CScopeLock		scopeLock( &synchronizationObject );
		return usedMalloc->Realloc( InOriginal, InSize, InAlignment );
	}

	/**
	 * @brief Deallocates the space previously allocated by Malloc or Realloc
	 * @param InOriginal	Pointer to the memory to deallocate
	 */
	virtual void Free( void* InOriginal ) override
	{
		if ( InOriginal )
		{
			CScopeLock		scopeLock( &synchronizationObject );
			usedMalloc->Free( InOriginal );
		}
	}

	/**
	* @brief If possible determine the size of the memory allocated at the given address
	*
	* @param InOriginal		Pointer to memory we are checking the size of
	* @param OutSize		If possible, this value is set to the size of the passed in pointer
	* @return Return TRUE if succeeded, otherwise returns FALSE
	*/
	virtual bool GetAllocationSize( void* InOriginal, size_t& OutSize )
	{
		CScopeLock		scopeLock( &synchronizationObject );
		return usedMalloc->GetAllocationSize( InOriginal, OutSize );
	}

	/**
	* @brief Releases as much memory as possible. Must be called from the main thread
	* @param InIsTrimThreadCaches	Is need trim thread caches
	*/
	virtual void Trim( bool InIsTrimThreadCaches )
	{
		CScopeLock		scopeLock( &synchronizationObject );
		usedMalloc->Trim( InIsTrimThreadCaches );
	}

	/**
	 * @brief Returns if the allocator is guaranteed to be thread-safe and therefore
	 * doesn't need a unnecessary thread-safety wrapper around it
	 * 
	 * @return Return TRUE if the allocator is guaranteed to be thread-safe, otherwise returns FALSE
	 */
	virtual bool IsInternallyThreadSafe() const
	{
		return true;
	}

	/**
	 * @brief Gets descriptive name for logging purposes
	 * @return Return pointer to human-readable malloc name
	 */
	virtual const tchar* GetDescriptiveName() const
	{
		CScopeLock		scopeLock( &synchronizationObject );
		return usedMalloc->GetDescriptiveName();
	}

private:
	CBaseMalloc*		usedMalloc;				/**< Malloc we're based on, aka using under the hood */
	mutable CMutex		synchronizationObject;	/**< Object used for synchronization via a scoped lock */
};

#endif // !MALLOCTHREADSAFEPROXY_H