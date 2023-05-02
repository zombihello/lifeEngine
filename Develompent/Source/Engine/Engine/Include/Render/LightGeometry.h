/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LIGHTGEOMETRY_H
#define LIGHTGEOMETRY_H

#include "RenderResource.h"
#include "RenderUtils.h"
#include "Misc/RefCounted.h"
#include "Render/VertexFactory/LightVertexFactory.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Light sphere mesh (only for point lights)
 */
class CLightSphereMesh : public CRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	CLightSphereMesh();

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
	 * @return Return RHI vertex buffer, if not created then return nullptr
	 */
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * @brief Get RHI index buffer
	 * @return Return RHI index buffer, if not created then return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

	/**
	 * @brief Get vertex factory
	 * @return Return vertex factory, if not created then return nullptr
	 */
	FORCEINLINE TRefCountPtr<CLightVertexFactory> GetVertexFactory() const
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
	uint32									numPrimitives;		/**< Number primitives */
	VertexBufferRHIRef_t					vertexBufferRHI;	/**< Vertex buffer RHI */
	IndexBufferRHIRef_t						indexBufferRHI;		/**< Index buffer RHI */
	TRefCountPtr<CLightVertexFactory>		vertexFactory;		/**< Vertex factory */
};

/**
 * @ingroup Engine
 * @brief Light quad mesh (only for directionl lights)
 */
class CLightQuadMesh : public CRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	CLightQuadMesh();

	/**
	 * @brief Get number primitives
	 * @return Return number primitives
	 */
	FORCEINLINE uint32 GetNumPrimitives() const
	{
		return 2;
	}

	/**
	 * @brief Get RHI vertex buffer
	 * @return Return RHI vertex buffer, if not created then return nullptr
	 */
	FORCEINLINE VertexBufferRHIRef_t GetVertexBufferRHI() const
	{
		return vertexBufferRHI;
	}

	/**
	 * @brief Get RHI index buffer
	 * @return Return RHI index buffer, if not created then return nullptr
	 */
	FORCEINLINE IndexBufferRHIRef_t GetIndexBufferRHI() const
	{
		return indexBufferRHI;
	}

	/**
	 * @brief Get vertex factory
	 * @return Return vertex factory, if not created then return NULL
	 */
	FORCEINLINE TRefCountPtr<CLightVertexFactory> GetVertexFactory() const
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
	VertexBufferRHIRef_t					vertexBufferRHI;	/**< Vertex buffer RHI */
	IndexBufferRHIRef_t						indexBufferRHI;		/**< Index buffer RHI */
	TRefCountPtr<CLightVertexFactory>		vertexFactory;		/**< Vertex factory */
};

extern TGlobalResource<CLightSphereMesh>	GLightSphereMesh;	/**< The global light sphere mesh data */
extern TGlobalResource<CLightQuadMesh>		GLightQuadMesh;		/**< The global light quad mesh data */

#endif // !LIGHTGEOMETRY_H