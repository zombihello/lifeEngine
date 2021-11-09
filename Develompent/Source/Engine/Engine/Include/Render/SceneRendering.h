/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENERENDERING_H
#define SCENERENDERING_H

#include "Render/Shaders/Shader.h"
#include "Render/DrawingPolicy.h"

/**
 * @ingroup Engine
 * Draw policy of static mesh
 */
class FStaticMeshDrawPolicy : public FMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 *
	 * @param[in] InVertexFactory Vertex factory
	 * @param[in] InMaterial Material
	 * @param[in] InDepthBias Depth bias
	 */
	FStaticMeshDrawPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias = 0.f );

	/**
	 * Set shader parameters
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI ) override;

	/**
	 * Draw mesh
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 * @param[in] InMeshBatch Mesh batch to draw
	 * @param[in] InSceneView Scene view
	 */
	virtual void Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMeshBatch, const class FSceneView& InSceneView ) override;
};

#endif // !SCENERENDERING_H