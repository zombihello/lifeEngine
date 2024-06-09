#include "System/MallocStd.h"
#include "Core.h"

/*
==================
CMallocStd::CMallocStd
==================
*/
CMallocStd::CMallocStd()
{
#if PLATFORM_WINDOWS
	// Enable low fragmentation heap http://msdn2.microsoft.com/en-US/library/aa366750.aspx
	intptr_t	crtHeapHandle	= _get_heap_handle();
	ULONG		enableLFH		= 2;
	HeapSetInformation( ( void* )crtHeapHandle, HeapCompatibilityInformation, &enableLFH, sizeof( enableLFH ) );
#endif // PLATFORM_WINDOWS
}

/*
==================
CMallocStd::Malloc
==================
*/
void* CMallocStd::Malloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryMalloc( InSize, InAlignment );
	if ( !result && InSize )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocStd::TryMalloc
==================
*/
void* CMallocStd::TryMalloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
	void*		result = nullptr;

	// Allocate memory
#if PLATFORM_USE__ALIGNED_MALLOC
	result	= _aligned_malloc( InSize, InAlignment );
#else
	void*	ptr = malloc( InSize + InAlignment + sizeof( void* ) + sizeof( size_t ) );
	if ( ptr )
	{
		result = Align( ( uint8* )ptr + sizeof( void* ) + sizeof( size_t ), InAlignment );
		*( ( void** )( ( uint8* )result - sizeof( void* ) ) ) = ptr;
		*( ( size_t* )( ( uint8* )result - sizeof( void* ) - sizeof( size_t ) ) ) = InSize;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
	
	return result;
}

/*
==================
CMallocStd::Realloc
==================
*/
void* CMallocStd::Realloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryRealloc( InOriginal, InSize, InAlignment );
	if ( !result && InSize != 0 )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocStd::TryRealloc
==================
*/
void* CMallocStd::TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	InAlignment = Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment );
	void*		result = nullptr;
	
#if PLATFORM_USE__ALIGNED_MALLOC
	if ( InOriginal && InSize )
	{
		result = _aligned_realloc( InOriginal, InSize, InAlignment );
	}
	else if ( !InOriginal )
	{
		result = _aligned_malloc( InSize, InAlignment );
	}
	else
	{
		_aligned_free( InOriginal );
		result = nullptr;
	}
#else
	if ( InOriginal && InSize )
	{
		// Can't use realloc as it might screw with alignment
		result = TryMalloc( InSize, InAlignment );
		size_t	ptrSize = 0;
		bool	bResult = GetAllocationSize( InOriginal, ptrSize );
		Assert( ptrSize );
		Memory::Memcpy( result, InOriginal, Min( InSize, ptrSize ) );
		Free( InOriginal );
	}
	else if ( !InOriginal )
	{
		result = TryMalloc( InSize, InAlignment );
	}
	else
	{
		free( *( ( void** )( ( uint8* )InOriginal - sizeof( void* ) ) ) );
		result = nullptr;
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC

	return result;
}

/*
==================
CMallocStd::Free
==================
*/
void CMallocStd::Free( void* InOriginal )
{
#if PLATFORM_USE__ALIGNED_MALLOC
	_aligned_free( InOriginal );
#else
	if ( InOriginal )
	{
		free( *( ( void** )( ( uint8* )InOriginal - sizeof( void* ) ) ) );
	}
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMallocStd::GetAllocationSize
==================
*/
bool CMallocStd::GetAllocationSize( void* InOriginal, size_t& OutSize )
{
	if ( !InOriginal )
	{
		return false;
	}

#if PLATFORM_USE__ALIGNED_MALLOC
	OutSize = _aligned_msize( InOriginal, 16, 0 ); // Assumes alignment of 16
	return true;
#else
	OutSize = *( ( size_t* )( ( uint8* )InOriginal - sizeof( void* ) - sizeof( size_t ) ) );
	return true;
#endif // PLATFORM_USE__ALIGNED_MALLOC
}

/*
==================
CMallocStd::IsInternallyThreadSafe
==================
*/
bool CMallocStd::IsInternallyThreadSafe() const
{
#if PLATFORM_IS_STD_MALLOC_THREADSAFE
	return true;
#else
	return false;
#endif
}