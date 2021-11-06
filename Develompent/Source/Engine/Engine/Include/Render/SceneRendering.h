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
 * Draw policy of test geometry
 */
class FTestDrawPolicy : public FMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 *
	 * @param[in] InMaterial Material
	 * @param[in] InVertexFactory Vertex factory
	 * @param[in] InDepthBias Depth bias
	 */
	FTestDrawPolicy( class FMaterial* InMaterial, class FVertexFactory* InVertexFactory, float InDepthBias = 0.f );

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
	 * @param[in] InSceneView Scene view
	 */
	virtual void Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FSceneView& InSceneView ) override;

private:
	FShaderRef			pixelShader;		/**< Pixel shader */
};

#endif // !SCENERENDERING_H