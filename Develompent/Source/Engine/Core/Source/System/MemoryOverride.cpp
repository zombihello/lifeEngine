#include "System/Memory.h"

/*
==================
operator new
==================
*/
void* operator new( size_t numBytes )
{
	return Memory::Malloc( numBytes );
}

/*
==================
operator delete
==================
*/
void operator delete( void* pPtr ) noexcept
{
	Memory::Free( pPtr );
}

/*
==================
operator new[]
==================
*/
void* operator new[]( size_t numBytes )
{
	return Memory::Malloc( numBytes );
}

/*
==================
operator delete[]
==================
*/
void operator delete[]( void* pPtr ) noexcept
{
	Memory::Free( pPtr );
}