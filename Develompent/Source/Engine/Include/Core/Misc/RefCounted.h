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

#ifndef REFCOUNTED_H
#define REFCOUNTED_H

#include "Core/Core.h"
#include "Core/Misc/Types.h"
#include "Core/System/Threading.h"

/**
 * @ingroup Core
 * @brief Object reference counting class
 */
class CRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	CRefCounted();

	/**
	 * @brief Destructor
	 */
	virtual ~CRefCounted();

	/**
	 * @brief Increment reference count
	 */
	FORCEINLINE void AddRef()					
	{ 
		Sys_InterlockedIncrement( ( int32* )&countReferences );
	}

	/**
	 * @brief Decrement reference count and delete self if no more references
	 */
	void ReleaseRef();

	/**
	 * @brief Get reference count
	 * @return Return reference count
	 */
	FORCEINLINE uint32 GetRefCount() const		
	{ 
		return countReferences; 
	}

private:
	uint32			countReferences;			/**< Count references on object */
};

#endif // !REFCOUNTED_H
