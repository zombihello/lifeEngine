#include "System/Memory.h"

/*
==================
operator new
==================
*/
void* operator new( size_t InNumBytes )
{
	return Memory::Malloc( InNumBytes );
}

/*
==================
operator delete
==================
*/
void operator delete( void* InPtr ) noexcept
{
	Memory::Free( InPtr );
}

/*
==================
operator new[]
==================
*/
void* operator new[]( size_t InNumBytes )
{
	return Memory::Malloc( InNumBytes );
}

/*
==================
operator delete[]
==================
*/
void operator delete[]( void* InPtr ) noexcept
{
	Memory::Free( InPtr );
}