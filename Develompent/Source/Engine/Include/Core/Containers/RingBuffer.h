/**
 * @file
 * @addtogroup Core Core
 *
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

#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include "Core/Misc/Types.h"
#include "Core/System/Threading.h"
#include "Core/Core.h"

/**
 * @ingroup Core
 * @brief A ring buffer for use with two threads: a reading thread and a writing thread
 */
class CRingBuffer
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InBufferSize	The size of the data buffer to allocate
	 * @param InAlignment	Alignment of each allocation unit (in bytes)
	 */
	CRingBuffer( uint32 InBufferSize, uint32 InAlignment = 1 );

	/**
	 * @brief Destructor
	 */
	~CRingBuffer();

	/**
	 * @brief A reference to an allocated chunk of the ring buffer
	 * Upon destruction of the context, the chunk is committed as written
	 */
	class CAllocationContext
	{
	public:
		/**
		 * @brief Upon construction, AllocationContext allocates a chunk from the ring buffer
		 * 
		 * @param InRingBuffer		The ring buffer to allocate from
		 * @param InAllocationSize	The size of the allocation to make
		 */
		CAllocationContext( CRingBuffer& InRingBuffer, uint32 InAllocationSize );

		/**
		 * @brief Upon destruction, the allocation is committed, if Commit hasn't been called manually
		 */
		~CAllocationContext();

		/**
		 * @brief Commits the allocated chunk of memory to the ring buffer
		 */
		void Commit();

		/**
		 * @brief Get allocation
		 * @return Return pointer to start allocation
		 */
		FORCEINLINE void* GetAllocation() const 
		{
			return allocationStart;
		}

		/**
		 * @brief Get allocated size
		 * @return Return allocated size
		 */
		FORCEINLINE uint32 GetAllocatedSize() const 
		{
			return ( uint32 )( allocationEnd - allocationStart );
		}

	private:
		CRingBuffer&		ringBuffer;			/**< Reference to ring buffer */
		byte*				allocationStart;	/**< Pointer to start allocation data */
		byte*				allocationEnd;		/**< Pointer to end allocation data */
	};

	/**
	 * @brief Checks if there is data to be read from the ring buffer, and if so accesses the pointer to the data to be read
	 * 
	 * @param OutReadPointer	When returning TRUE, this will hold the pointer to the data to read
	 * @param OutReadSize		When returning TRUE, this will hold the number of bytes available to read
	 * @return Return TRUE if there is data to be read, otherwise returns FALSE
	 */
	bool BeginRead( void*& OutReadPointer, uint32& OutReadSize );

	/**
	 * @brief Frees the first ReadSize bytes available for reading via BeginRead to the writing thread
	 * @param InReadSize	The number of bytes to free
	 */
	void FinishRead( uint32 InReadSize );

	/**
	 * @brief Waits for data to be available for reading
	 * @param InWaitTime	Time in milliseconds to wait before returning
	 */
	void WaitForRead( uint32 InWaitTime = ( uint32 )-1 );

	/**
	 * @brief Checks if some data has been written to or not
	 * @return Return TRUE if the buffer isn't empty, otherwise returns FALSE
	 */
	FORCEINLINE bool IsReadBufferEmpty() const
	{
		return readPointer == writePointer;
	}

private:
	byte*				data;					/**< Data buffer */
	byte*				dataEnd;				/**< The first byte after end the of the data buffer */
	byte* volatile		writePointer;			/**< The next byte to be written to */
	bool				isWriting;				/**< TRUE if there is an AllocationContext outstanding for this ring buffer */
	byte* volatile		readPointer;			/**< The next byte to be read from */
	uint32				alignment;				/**< Alignment of each allocation unit (in bytes) */
	CThreadEvent		dataWrittenEvent;		/**< The event used to signal the reader thread when the ring buffer has data to read */
};

#endif // !RINGBUFFER_H
