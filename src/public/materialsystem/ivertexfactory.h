/**
 * @file
 * @addtogroup materialsystem materialsystem
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

#ifndef IVERTEXFACTORY_H
#define IVERTEXFACTORY_H

#include "core/types.h"
#include "stdlib/refcount.h"
#include "studiorender/istudiorenderresource.h"

/**
 * @ingroup materialsystem
 * @brief Enumeration of all vertex factory types
 */
enum EVertexFactoryType
{
	VERTEXFACTORY_TYPE_SIMPLEELEMENT,	/**< Vertex factory for simple elements */
	VERTEXFACTORY_NUM_TYPES				/**< Number of vertex factory types */
};

/**
 * @ingroup materialsystem
 * @brief Vertex factory interface
 */
class IVertexFactory : public IStudioRenderResource, public IRefCounted
{
public:
	/**
	 * @brief Initialize vertex factory
	 */
	virtual void Init() = 0;

	/**
	 * @brief Shutdown vertex factory
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Get vertex factory type
	 * @return Return vertex factory type
	 */
	virtual EVertexFactoryType GetType() const = 0;

	/**
	 * @brief Is supports instancing
	 * @return Return TRUE if this vertex factory is supported instancing, otherwise returns FALSE
	 */
	virtual bool SupportsInstancing() const = 0;

	/**
	 * @brief Get instance stream index
	 * @return Return instance stream index
	 */
	virtual uint32 GetInstanceStreamIndex() const = 0;
};

#endif // !IVERTEXFACTORY_H