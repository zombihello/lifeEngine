#include "Core.h"
#include "System/MallocStd.h"
#include "System/MallocMimalloc.h"
#include "WindowsMemory.h"

/*
==================
WindowsPlatformMemory::AllocDefaultAllocator
==================
*/
CBaseMalloc* WindowsPlatformMemory::AllocDefaultAllocator()
{
#if PLATFORM_SUPPORTS_MIMALLOC
	// Mimalloc default allocator because it has great performance
	allocatorToUse = GenericPlatformMemory::MAU_Mimalloc;
	return new CMallocMimalloc();
#endif // PLATFORM_SUPPORTS_MIMALLOC

	// Fallback allocator
	return new CMallocStd();
}