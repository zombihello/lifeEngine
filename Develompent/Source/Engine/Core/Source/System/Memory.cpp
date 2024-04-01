#include "System/Memory.h"
#include "System/MallocThreadSafeProxy.h"
#include "Core.h"

//
// GLOBALS
//
CBaseMalloc* g_Malloc = nullptr;

/*
==================
Memory::CreateGlobalMalloc
==================
*/
void Memory::CreateGlobalMalloc()
{
	Assert( !g_Malloc );
	g_Malloc = PlatformMemory::AllocDefaultAllocator();

	// If the allocator is already thread safe, there is no need for the thread safe proxy
	if ( !g_Malloc->IsInternallyThreadSafe() )
	{
		g_Malloc = new CMallocThreadSafeProxy( g_Malloc );
	}
}