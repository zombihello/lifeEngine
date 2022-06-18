/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SPHERE_H
#define SPHERE_H

#include "RenderResource.h"
#include "RenderUtils.h"
#include "Misc/RefCounted.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

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

extern TGlobalResource< CSphereMesh >		GSphereMesh;			/**< The global sphere mesh data */

#endif // !SPHERE_H