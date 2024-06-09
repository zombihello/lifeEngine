#include "System/BaseMalloc.h"
#include "System/Memory.h"

/*
==================
CUseSystemMallocForNew::operator new
==================
*/
void* CUseSystemMallocForNew::operator new( size_t InSize )
{
	return Memory::SystemMalloc( InSize );
}

/*
==================
CUseSystemMallocForNew::operator delete
==================
*/
void CUseSystemMallocForNew::operator delete( void* InPtr )
{
	Memory::SystemFree( InPtr );
}

/*
==================
CUseSystemMallocForNew::operator new[]
==================
*/
void* CUseSystemMallocForNew::operator new[]( size_t InSize )
{
	return Memory::SystemMalloc( InSize );
}

/*
==================
CUseSystemMallocForNew::operator delete[]
==================
*/
void CUseSystemMallocForNew::operator delete[]( void* InPtr )
{
	Memory::SystemFree( InPtr );
}