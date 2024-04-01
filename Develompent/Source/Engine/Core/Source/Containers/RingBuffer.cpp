#include "Containers/RingBuffer.h"
#include "System/Threading.h"
#include "Misc/Template.h"

/* Critical section of ring buffer */
static CThreadMutex			s_RingBufferMutex;

/*
==================
CRingBuffer::CRingBuffer
==================
*/
CRingBuffer::CRingBuffer( uint32 InBufferSize, uint32 InAlignment /*= 1*/ ) 
	: alignment( InAlignment )
{
	data		= new byte[InBufferSize];
	dataEnd		= data + InBufferSize;
	readPointer = writePointer = data;
}

/*
==================
CRingBuffer::~CRingBuffer
==================
*/
CRingBuffer::~CRingBuffer()
{
	delete[] data;	
}

/*
==================
CRingBuffer::CAllocationContext::CAllocationContext
==================
*/
CRingBuffer::CAllocationContext::CAllocationContext( CRingBuffer& InRingBuffer, uint32 InAllocationSize ) 
	: ringBuffer( InRingBuffer )
{
	s_RingBufferMutex.Lock();

	// Only allow a single AllocationContext at a time for the ring buffer
	Assert( !ringBuffer.isWriting );
	ringBuffer.isWriting = true;

	// Check that the allocation will fit in the buffer
	const uint32		alignedAllocationSize = Align( InAllocationSize, ringBuffer.alignment );
	const uint32		bufferSize = ( uint32 )( ringBuffer.dataEnd - ringBuffer.data );
	Assert( alignedAllocationSize < bufferSize );

	// Use the memory referenced by WritePointer for the allocation, wrapped around to the beginning of the buffer
	// if it was at the end
	allocationStart = ringBuffer.writePointer != ringBuffer.dataEnd ? ringBuffer.writePointer : ringBuffer.data;

	// If there isn't enough space left in the buffer to allocate the full size, allocate all the remaining bytes in the buffer
	allocationEnd = Min( ringBuffer.dataEnd, allocationStart + alignedAllocationSize );

	// Wait until the reading thread has finished reading the area of the buffer we want to allocate
	while ( true )
	{
		// Make a snapshot of a recent value of ReadPointer
		byte*		currentReadPointer = ringBuffer.readPointer;

		// If the ReadPointer and WritePointer are the same, the buffer is empty and there's no risk of overwriting unread data
		if ( currentReadPointer == ringBuffer.writePointer )
		{
			break;
		}
		else
		{
			// If the allocation doesn't contain the read pointer, the allocation won't overwrite unread data.
			// Note that it needs to also prevent advancing WritePointer to match the current ReadPointer, since that would signal that the
			// buffer is empty instead of the expected full
			const bool		allocationContainsReadPointer = allocationStart <= currentReadPointer && currentReadPointer <= allocationEnd;
			if ( !allocationContainsReadPointer )
			{
				break;
			}
		}

		Sys_Sleep( 0.1f );
	}
}

/*
==================
CRingBuffer::CAllocationContext::~CAllocationContext
==================
*/
CRingBuffer::CAllocationContext::~CAllocationContext()
{
	Commit();
}

/*
==================
CRingBuffer::CAllocationContext::Commit
==================
*/
void CRingBuffer::CAllocationContext::Commit()
{
	if ( allocationStart )
	{
		// Advance the write pointer to the next unallocated byte
		ringBuffer.writePointer = allocationEnd;

		// Reset the IsWriting flag to allow other AllocationContexts to be created for the ring buffer
		ringBuffer.isWriting = false;
		s_RingBufferMutex.Unlock();

		// Clear the allocation pointer, to signal that it has been committed
		allocationStart = nullptr;

		// Trigger the data-written event to wake the reader thread
		ringBuffer.dataWrittenEvent.Trigger();
	}
}

/*
==================
CRingBuffer::BeginRead
==================
*/
bool CRingBuffer::BeginRead( void*& OutReadPointer, uint32& OutReadSize )
{
	// Make a snapshot of a recent value of WritePointer, and use a memory barrier to ensure that reads from the data buffer
	// will see writes no older than this snapshot of the WritePointer
	byte*	currentWritePointer = writePointer;

	// Determine whether the write pointer or the buffer end should delimit this contiguous read
	byte*	readEndPointer		= nullptr;
	if ( currentWritePointer >= readPointer )
	{
		readEndPointer = currentWritePointer;
	}
	else
	{
		// If the read pointer has reached the end of readable data in the buffer, reset it to the beginning of the buffer
		if ( readPointer == dataEnd )
		{
			readPointer		= data;
			readEndPointer	= currentWritePointer;
		}
		else
		{
			readEndPointer	= dataEnd;
		}
	}

	// Determine whether there's data to read, and how much
	if ( readPointer < readEndPointer )
	{
		OutReadPointer	= readPointer;
		OutReadSize		= ( uint32 )( readEndPointer - readPointer );
		return true;
	}

	return false;
}

/*
==================
CRingBuffer::FinishRead
==================
*/
void CRingBuffer::FinishRead( uint32 InReadSize )
{
	readPointer += Align( InReadSize, alignment );
}

/*
==================
CRingBuffer::WaitForRead
==================
*/
void CRingBuffer::WaitForRead( uint32 InWaitTime /*= (uint32)-1*/ )
{
	// If the buffer is empty, wait for the data-written event to be triggered
	if ( readPointer == writePointer )
	{
		dataWrittenEvent.Wait( InWaitTime );
	}
}