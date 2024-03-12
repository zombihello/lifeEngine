/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "pch_studiorender.h"
#include "studiorender/studiocmdbuffer.h"

/**
 * @ingroup studiorender
 * @brief Mutex of studio command buffer
 */
static CThreadMutex		s_StudioCmdBufferMutex;

/*
==================
CStudioCmdBuffer::CStudioCmdBuffer
==================
*/
CStudioCmdBuffer::CStudioCmdBuffer( uint32 bufferSize, uint32 alignment /* = 1 */ )
	: pData( nullptr )
	, pDataEnd( nullptr )
	, pWritePointer( nullptr )
	, bWriting( false )
	, pReadPointer( nullptr )
	, alignment( alignment )
{
	pData			= new byte[bufferSize];
	pDataEnd		= pData + bufferSize;
	pReadPointer	= pWritePointer = pData;
}

/*
==================
CStudioCmdBuffer::~CStudioCmdBuffer
==================
*/
CStudioCmdBuffer::~CStudioCmdBuffer()
{
	delete[] pData;
}

/*
==================
CStudioCmdBuffer::GetAllocation
==================
*/
StudioCmdAlloc CStudioCmdBuffer::GetAllocation( uint32 allocationSize )
{
	s_StudioCmdBufferMutex.Lock();

	// Only allow a single allocation chunk at a time
	AssertMsg( !bWriting, "CStudioRender: Only allow a single allocation chunk at a time" );
	bWriting			= true;
	StudioCmdAlloc		studioCmdAlloc;
	Mem_Memzero( &studioCmdAlloc, sizeof( StudioCmdAlloc ) );

	// Check that the allocation will fit in the buffer
	const uint32	alignedAllocationSize	= Align( allocationSize, alignment );
	const uint32	bufferSize				= ( uint32 )( pDataEnd - pData );
	AssertMsg( alignedAllocationSize < bufferSize, "CStudioRender: No enough space for the allocation in the command buffer" );

	// Use the memory referenced by WritePointer for the allocation, wrapped around to the beginning of the buffer
	// if it was at the end
	studioCmdAlloc.pAllocation = pWritePointer != pDataEnd ? pWritePointer : pData;

	// If there isn't enough space left in the buffer to allocate the full size, allocate all the remaining bytes in the buffer
	byte*	 pAllocationEnd			= Min( pDataEnd, studioCmdAlloc.pAllocation + alignedAllocationSize );
	studioCmdAlloc.allocatedSize	= ( uint32 )( pAllocationEnd - studioCmdAlloc.pAllocation );

	// Wait until the reading thread has finished reading the area of the buffer we want to allocate
	while ( true )
	{
		// Make a snapshot of a recent value of ReadPointer
		byte*	pCurReadPointer = pReadPointer;

		// If the ReadPointer and WritePointer are the same, the buffer is empty and there's no risk of overwriting unread data
		if ( pCurReadPointer == pWritePointer )
		{
			break;
		}
		else
		{
			// If the allocation doesn't contain the read pointer, the allocation won't overwrite unread data
			// Note that it needs to also prevent advancing WritePointer to match the current ReadPointer, since that would signal that the
			// buffer is empty instead of the expected full
			const bool		bAllocationContainsReadPointer = studioCmdAlloc.pAllocation <= pCurReadPointer && pCurReadPointer <= pAllocationEnd;
			if ( !bAllocationContainsReadPointer )
			{
				break;
			}
		}

		Sys_Yield();
	}

	return studioCmdAlloc;
}

/*
==================
CStudioCmdBuffer::CommitAllocation
==================
*/
void CStudioCmdBuffer::CommitAllocation( StudioCmdAlloc& allocContext )
{
	if ( allocContext.pAllocation )
	{
		// Advance the write pointer to the next unallocated byte
		pWritePointer = allocContext.pAllocation + allocContext.allocatedSize;

		// Reset the bWriting flag to allow other chunk to be allocated
		bWriting = false;
		s_StudioCmdBufferMutex.Unlock();

		// Clear the allocation pointer, to signal that it has been committed
		allocContext.pAllocation = nullptr;

		// Trigger the data-written event to wake the reader thread
		dataWrittenEvent.Trigger();
	}
}

/*
==================
CStudioCmdBuffer::Flush
==================
*/
void CStudioCmdBuffer::Flush()
{
	while ( !IsReadBufferEmpty() )
	{
		Sys_Yield();
	}
}

/*
==================
CStudioCmdBuffer::BeginRead
==================
*/
bool CStudioCmdBuffer::BeginRead( void*& pReadPointer, uint32& readSize )
{
	// Make a snapshot of a recent value of WritePointer
	byte*	pCurWritePointer = pWritePointer;

	// Do nothing if read buffer is empty
	if ( IsReadBufferEmpty() )
	{
		return false;
	}

	// Determine whether the write pointer or the buffer end should delimit this contiguous read
	byte*	pReadEndPointer = nullptr;
	if ( pCurWritePointer >= CStudioCmdBuffer::pReadPointer )
	{
		pReadEndPointer = pCurWritePointer;
	}
	else
	{
		// If the read pointer has reached the end of readable data in the buffer, reset it to the beginning of the buffer
		if ( CStudioCmdBuffer::pReadPointer == pDataEnd )
		{
			CStudioCmdBuffer::pReadPointer	= pData;
			pReadEndPointer					= pCurWritePointer;
		}
		else
		{
			pReadEndPointer = pDataEnd;
		}
	}

	// Determine whether there's data to read, and how much
	if ( CStudioCmdBuffer::pReadPointer < pReadEndPointer )
	{
		pReadPointer	= CStudioCmdBuffer::pReadPointer;
		readSize		= ( uint32 )( pReadEndPointer - CStudioCmdBuffer::pReadPointer );
		return true;
	}

	return false;
}

/*
==================
CStudioCmdBuffer::EndRead
==================
*/
void CStudioCmdBuffer::EndRead( uint32 readSize )
{
	pReadPointer += Align( readSize, alignment );
}

/*
==================
CStudioCmdBuffer::WaitForRead
==================
*/
void CStudioCmdBuffer::WaitForRead( uint32 waitTime /* = -1 */ )
{
	// If the buffer is empty, wait for the data-written event to be triggered
	if ( pReadPointer == pWritePointer )
	{
		dataWrittenEvent.Wait( waitTime );
	}
}