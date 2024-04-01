/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MEMORYOVERRIDE_H
#define MEMORYOVERRIDE_H

// Override the global memory allocator
// NOTE: Override new and delete in the file MemoryOverride.cpp
#undef malloc
#define malloc( NumBytes )			Memory::Malloc( NumBytes )

#undef realloc
#define realloc( Ptr, NumBytes )	Memory::Realloc( Ptr, NumBytes )

#undef free
#define free( Ptr )					Memory::Free( Ptr )

#endif // !MEMORYOVERRIDE_H