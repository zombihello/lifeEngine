/**
 * @file
 * @addtogroup studiorender studiorender
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

#ifndef ISTUDIOCMDBUFFER_H
#define ISTUDIOCMDBUFFER_H

#include "core/types.h"
#include "core/platform.h"

// Forward declarations
class IStudioCmdBuffer;

/**
 * @ingroup studiorender
 * @brief A reference to an allocated chunk of IStudioCmdBuffer
 */
struct StudioCmdAlloc
{
	byte*	pAllocation;	/**< Pointer to the begin allocation data */
	uint32	allocatedSize;	/**< Allocated size */
};

/**
 * @ingroup studiorender
 * @brief Studio render command buffer interface
 */
class IStudioCmdBuffer
{
public:
	/**
	 * @brief Allocate a chunk
	 * @warning After filling in the allocated chunk you have to call CommitAllocation
	 * 
	 * @param allocationSize	The size of the allocation to make.
	 * @return Return context of allocated chunk
	 */
	virtual StudioCmdAlloc GetAllocation( uint32 allocationSize ) = 0;

	/**
	 * @brief Commit the allocated chunk
	 * @param allocContext	Allocated chunk context
	 */
	virtual void CommitAllocation( StudioCmdAlloc& allocContext ) = 0;

	/**
	 * @brief Flush render commands
	 */
	virtual void Flush() = 0;
};

#endif // !ISTUDIOCMDBUFFER_H