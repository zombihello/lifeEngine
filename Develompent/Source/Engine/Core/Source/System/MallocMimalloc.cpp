#include "System/MallocMimalloc.h"
#include "Core.h"

#if PLATFORM_SUPPORTS_MIMALLOC
#include <mimalloc.h>

/*
==================
CMallocMimalloc::Malloc
==================
*/
void* CMallocMimalloc::Malloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
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
CMallocMimalloc::TryMalloc
==================
*/
void* CMallocMimalloc::TryMalloc( size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	newPtr = nullptr;
	if ( InAlignment != DEFAULT_ALIGNMENT )
	{
		newPtr = mi_malloc_aligned( InSize, Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment ) );
	}
	else
	{
		newPtr = mi_malloc_aligned( InSize, InSize >= 16 ? 16 : 8 );
	}
	return newPtr;
}

/*
==================
CMallocMimalloc::Realloc
==================
*/
void* CMallocMimalloc::Realloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	result = TryRealloc( InOriginal, InSize, InAlignment );
	if ( !result && InSize )
	{
		PlatformMemory::OutOfMemory( InSize, InAlignment );
	}

	return result;
}

/*
==================
CMallocMimalloc::TryRealloc
==================
*/
void* CMallocMimalloc::TryRealloc( void* InOriginal, size_t InSize, uint32 InAlignment /* = DEFAULT_ALIGNMENT */ )
{
	void*	newPtr = nullptr;
	if ( InSize == 0 )
	{
		mi_free( InOriginal );
		return nullptr;
	}

	if ( InAlignment != DEFAULT_ALIGNMENT )
	{
		newPtr = mi_realloc_aligned( InOriginal, InSize, Max<uint32>( InSize >= 16 ? 16 : 8, InAlignment ) );
	}
	else
	{
		newPtr = mi_realloc( InOriginal, InSize );
	}

	return newPtr;
}

/*
==================
CMallocMimalloc::Free
==================
*/
void CMallocMimalloc::Free( void* InOriginal )
{
	if ( !InOriginal )
	{
		return;
	}

	mi_free( InOriginal );
}

/*
==================
CMallocMimalloc::GetAllocationSize
==================
*/
bool CMallocMimalloc::GetAllocationSize( void* InOriginal, size_t& OutSize )
{
	OutSize = mi_malloc_size( InOriginal );
	return true;
}

/*
==================
CMallocMimalloc::Trim
==================
*/
void CMallocMimalloc::Trim( bool InIsTrimThreadCaches )
{
	mi_collect( InIsTrimThreadCaches );
}
#endif // PLATFORM_SUPPORTS_MIMALLOC