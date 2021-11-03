/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DRAWINGPOLICY_H
#define DRAWINGPOLICY_H

#include "Misc/RefCountPtr.h"
#include "Render/Material.h"
#include "Core.h"

/**
 * @ingroup Engine
 * The base mesh drawing policy.  Subclasses are used to draw meshes with type-specific context variables.
 * May be used either simply as a helper to render a dynamic mesh, or as a static instance shared between
 * similar meshs
 */
class FMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InMaterial Material
	 * @param[in] InDepthBias Depth bias
	 */
	FMeshDrawingPolicy( class FMaterial* InMaterial, float InDepthBias = 0.f );

	/**
	 * Destructor
	 */
	virtual ~FMeshDrawingPolicy();

	/**
	 * Set render state
	 * In this method setted FBoundShaderState, rasterizer state, etc
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 * @param[in] InVertexDeclaration Vertex declaration
	 */
	virtual void SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI, FVertexDeclarationRHIParamRef InVertexDeclaration );

	/**
	 * Set shader parameters
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * Draw mesh
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 * @param[in] InSceneView Scene view
	 */
	virtual void Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FSceneView& InSceneView ) PURE_VIRTUAL( FMeshDrawingPolicy::Draw, );

protected:
	FMaterialRef		material;		/**< Material */
	float				depthBias;		/**< Depth bias */
};

#endif // !DRAWINGPOLICY_H