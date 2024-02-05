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

#ifndef SPHERE_H
#define SPHERE_H

#include "Engine/Render/RenderResource.h"
#include "Engine/Render/RenderUtils.h"
#include "Core/Misc/RefCounted.h"
#include "Engine/Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Engine/Render/VertexFactory/LightVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine 
 * @brief Sphere mesh
 */
class CSphereMesh : public CRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	CSphereMesh();

	/**
	 * @brief Get number primitives
	 * @return Return number primitives
	 */
	FORCEINLINE uint32 GetNumPrimitives() const
	{
		return numPrimitives;
	}

	/**
	 * @brief Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created return nullptr
	 */
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * @brief Get RHI index buffer
	 * @return Return RHI index buffer, if not created return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

	/**
	 * @brief Get vertex factory
	 * @return Return vertex factory, if not created return NULL
	 */
	FORCEINLINE TRefCountPtr< CDynamicMeshVertexFactory > GetVertexFactory() const
	{
		return vertexFactory;
	}

protected:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	uint32											numPrimitives;		/**< Number primitives */
	VertexBufferRHIRef_t							vertexBufferRHI;	/**< Vertex buffer RHI */
	IndexBufferRHIRef_t								indexBufferRHI;		/**< Index buffer RHI */
	TRefCountPtr< CDynamicMeshVertexFactory >		vertexFactory;		/**< Vertex factory */
};

extern TGlobalResource<CSphereMesh>					g_SphereMesh;			/**< The global sphere mesh data */

#endif // !SPHERE_H