#include "Core.h"
#include "System/GenericPlatformMemory.h"
#include "System/MallocStd.h"

//
// GLOBALS
//
GenericPlatformMemory::EMemoryAllocatorToUse	GenericPlatformMemory::allocatorToUse = GenericPlatformMemory::MAU_Std;

/*
==================
GenericPlatformMemory::AllocDefaultAllocator
==================
*/
CBaseMalloc* GenericPlatformMemory::AllocDefaultAllocator()
{
	return new CMallocStd();
}

/*
==================
GenericPlatformMemory::OutOfMemory
==================
*/
void GenericPlatformMemory::OutOfMemory( size_t InSize, uint32 InAlignment )
{
	Sys_Errorf( TEXT( "Ran out of memory allocating %llu bytes with alignment %u" ), InSize, InAlignment );
}