#include "Containers/RingBuffer.h"
#include "System/ThreadingBase.h"
#include "Misc/Template.h"

/* Critical section of ring buffer */
static ÑCriticalSection			GCriticalSection;

ÑRingBuffer::ÑRingBuffer( uint32 InBufferSize, uint32 InAlignment /*= 1*/ ) :
	dataWrittenEvent( nullptr ),
	alignment( InAlignment )
{
	data = new byte[ InBufferSize ];
	dataEnd = data + InBufferSize;
	readPointer = writePointer = data;
}

ÑRingBuffer::~ÑRingBuffer()
{
	GSynchronizeFactory->Destroy( dataWrittenEvent );
	delete[] data;	
}

ÑRingBuffer::ÑAllocationContext::ÑAllocationContext( ÑRingBuffer& InRingBuffer, uint32 InAllocationSize ) :
	ringBuffer( InRingBuffer )
{
	GCriticalSection.Lock();

	// Only allow a single AllocationContext at a time for the ring buffer.
	check( !ringBuffer.isWriting );
	ringBuffer.isWriting = true;

	// Check that the allocation will fit in the buffer.
	const uint32		alignedAllocationSize = Align( InAllocationSize, ringBuffer.alignment );
	const uint32		bufferSize = ( uint32 )( ringBuffer.dataEnd - ringBuffer.data );
	check( alignedAllocationSize < bufferSize );

	// Use the memory referenced by WritePointer for the allocation, wrapped around to the beginning of the buffer
	// if it was at the end.
	allocationStart = ringBuffer.writePointer != ringBuffer.dataEnd ? ringBuffer.writePointer : ringBuffer.data;

	// If there isn't enough space left in the buffer to allocate the full size, allocate all the remaining bytes in the buffer.
	allocationEnd = Min( ringBuffer.dataEnd, allocationStart + alignedAllocationSize );

	// Wait until the reading thread has finished reading the area of the buffer we want to allocate
	while ( true )
	{
		// Make a snapshot of a recent value of ReadPointer.
		byte*		currentReadPointer = ringBuffer.readPointer;

		// If the ReadPointer and WritePointer are the same, the buffer is empty and there's no risk of overwriting unread data.
		if ( currentReadPointer == ringBuffer.writePointer )
		{
			break;
		}
		else
		{
			// If the allocation doesn't contain the read pointer, the allocation won't overwrite unread data.
			// Note that it needs to also prevent advancing WritePointer to match the current ReadPointer, since that would signal that the
			// buffer is empty instead of the expected full.
			const bool		allocationContainsReadPointer = allocationStart <= currentReadPointer && currentReadPointer <= allocationEnd;
			if ( !allocationContainsReadPointer )
			{
				break;
			}
		}

		appSleep( 0.1f );
	}
}

ÑRingBuffer::ÑAllocationContext::~ÑAllocationContext()
{
	Commit();
}

void ÑRingBuffer::ÑAllocationContext::Commit()
{
	if ( allocationStart )
	{
		// Advance the write pointer to the next unallocated byte.
		ringBuffer.writePointer = allocationEnd;

		// Reset the IsWriting flag to allow other AllocationContexts to be created for the ring buffer.
		ringBuffer.isWriting = false;
		GCriticalSection.Unlock();

		// Clear the allocation pointer, to signal that it has been committed.
		allocationStart = nullptr;

		// Lazily create the data-written event. It can't be done in the ÑRingBuffer constructor because GSynchronizeFactory may not
		// be initialized at that point.
		if ( !ringBuffer.dataWrittenEvent )
		{
			ringBuffer.dataWrittenEvent = GSynchronizeFactory->CreateSynchEvent();
			checkMsg( ringBuffer.dataWrittenEvent, TEXT( "Failed to create data-write event for ÑRingBuffer" ) );
		}

		// Trigger the data-written event to wake the reader thread.
		ringBuffer.dataWrittenEvent->Trigger();
	}
}

bool ÑRingBuffer::BeginRead( void*& OutReadPointer, uint32& OutReadSize )
{
	// Make a snapshot of a recent value of WritePointer, and use a memory barrier to ensure that reads from the data buffer
	// will see writes no older than this snapshot of the WritePointer.
	byte*			currentWritePointer = writePointer;

	// Determine whether the write pointer or the buffer end should delimit this contiguous read.
	byte*			readEndPointer = nullptr;
	if ( currentWritePointer >= readPointer )
	{
		readEndPointer = currentWritePointer;
	}
	else
	{
		// If the read pointer has reached the end of readable data in the buffer, reset it to the beginning of the buffer.
		if ( readPointer == dataEnd )
		{
			readPointer = data;
			readEndPointer = currentWritePointer;
		}
		else
		{
			readEndPointer = dataEnd;
		}
	}

	// Determine whether there's data to read, and how much.
	if ( readPointer < readEndPointer )
	{
		OutReadPointer = readPointer;
		OutReadSize = ( uint32 )( readEndPointer - readPointer );
		return true;
	}

	return false;
}

void ÑRingBuffer::FinishRead( uint32 InReadSize )
{
	readPointer += Align( InReadSize, alignment );
}

void ÑRingBuffer::WaitForRead( uint32 InWaitTime /*= (uint32)-1*/ )
{
	// If the buffer is empty, wait for the data-written event to be triggered.
	if ( readPointer == writePointer )
	{
		if ( dataWrittenEvent )
		{
			dataWrittenEvent->Wait( InWaitTime );
		}
		else
		{
			appSleep( 0.001f );			// Changed from 0 to 1ms (the shortest delay supported)
		}
	}
}