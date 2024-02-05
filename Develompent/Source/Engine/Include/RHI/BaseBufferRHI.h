/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef BASEBUFFERRHI_H
#define BASEBUFFERRHI_H

#include "Core/Core.h"
#include "Core/Misc/RefCounted.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Enumeration of resource usage flags for vertex and index buffers
 */
enum EResourceUsageFlags
{
	RUF_Static		= 1 << 0,							/**< The resource will be created, filled, and never repacked */
	RUF_Dynamic		= 1 << 1,							/**< The resource will be repacked in-frequently */
	RUF_Volatile	= 1 << 2,							/**< The resource will be repacked EVERY frame */

	RUF_AnyDynamic	= RUF_Dynamic | RUF_Volatile		/**< Helper bit-mask for dynamic */
};

/**
 * @ingroup Engine
 * @brief Information about a resource that is currently locked
 */
struct LockedData
{
	/**
	 * Constructor
	 */
	LockedData() :
		data( nullptr ),
		isNeedFree( false ),
		size( 0 ),
		pitch( 0 )
	{}

	/**
	 * Destructor
	 */
	~LockedData()
	{
		if ( isNeedFree )
		{
			delete[] data;
			isNeedFree = false;
		}

		data = nullptr;
	}

	byte*					data;				/**< Pointer to data */
	bool					isNeedFree;			/**< Need free data? */
	uint32					size;				/**< Size of data */
	uint32					pitch;				/**< Pitch of data */
	ResourceRHIRef_t		stagingResource;	/**< Staging resource for read/write operations */
};

/**
 * @ingroup Engine
 * @brief Base class for work with vertex buffer
 */
class CBaseVertexBufferRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InSize Size of buffer
	 */
	CBaseVertexBufferRHI( uint32 InUsage, uint32 InSize ) :
		usage( InUsage ),
		size( InSize )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseVertexBufferRHI() {}

	/**
	 * @brief Get usage flags
	 * @return Usage flags
	 */
	FORCEINLINE uint32 GetUsage() const { return usage; }

	/**
	 * @brief Get buffer size
	 * @return Buffer size
	 */
	FORCEINLINE uint32 GetSize() const { return size; }

private:
	uint32			usage;		/**< Usage flags */
	uint32			size;		/**< Size of buffer */
};

/**
 * @ingroup Engine
 * @brief Base class for work with index buffer
 */
class CBaseIndexBufferRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InStride Stride of struct
	 * @param[in] InSize Size of buffer
	 */
	CBaseIndexBufferRHI( uint32 InUsage, uint32 InStride, uint32 InSize )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseIndexBufferRHI() {}

	/**
	 * @brief Get usage flags
	 * @return Usage flags
	 */
	FORCEINLINE uint32 GetUsage() const { return usage; }

	/**
	 * @brief Get buffer size
	 * @return Buffer size
	 */
	FORCEINLINE uint32 GetSize() const { return size; }

	/**
	 * @brief Get stride
	 * @return Stride of struct
	 */
	FORCEINLINE uint32 GetStride() const { return stride; }

private:
	uint32			usage;		/**< Usage flags */
	uint32			stride;		/**< Stride of struct */
	uint32			size;		/**< Size of buffer */
};

#endif // !BASEBUFFERRHI_H