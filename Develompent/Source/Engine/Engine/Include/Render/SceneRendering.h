/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCENERENDERING_H
#define SCENERENDERING_H

#include "Math/Color.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "Render/Shaders/Shader.h"
#include "Render/DrawingPolicy.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * Draw policy of static mesh
 */
class FStaticMeshDrawPolicy : public FMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 */
	FStaticMeshDrawPolicy();

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

#if WITH_EDITOR
/**
 * @ingroup Engine
 * Drawing policy of wireframe meshes
 */
template< class TBaseMeshDrawingPolicy >
class TWireframeMeshDrawingPolicy : public TBaseMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 */
	TWireframeMeshDrawingPolicy()
		: wireframeColor( FColor::red )
	{}

	/**
	 * Constructor
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InDepthBias		Depth bias
	 */
	TWireframeMeshDrawingPolicy( class FVertexFactory* InVertexFactory, float InDepthBias = 0.f )
		: TBaseMeshDrawingPolicy( InVertexFactory, GEngine->GetDefaultWireframeMaterial(), InDepthBias )
		, wireframeColor( FColor::red )
	{}

	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InWireframeColor	Wireframe color
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class FVertexFactory* InVertexFactory, const FColor& InWireframeColor = FColor::red, float InDepthBias = 0.f )
	{
		FMaterialRef		wireframeMaterial = GEngine->GetDefaultWireframeMaterial();
		wireframeColor		= InWireframeColor;	
		wireframeMaterial->SetVectorParameterValue( TEXT( "wireframeColor" ), InWireframeColor.ToNormalizedVector4D() );		// TODO: For correct work need implement material instances
		
		InitInternal( InVertexFactory, wireframeMaterial, InDepthBias );
	}

private:
	FColor			wireframeColor;		/**< Wireframe color */
};
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * Class for render scene
 */
class FSceneRenderer
{
public:
	/**
	 * Constructor
	 * 
	 * @param InSceneView Scene view
	 */
	FSceneRenderer( FSceneView* InSceneView );

	/**
	 * Render scene
	 * 
	 * @param InViewportRHI Viewport
	 */
	void Render( FViewportRHIParamRef InViewportRHI );

private:
	FSceneView*			sceneView;		/**< Scene view */
};

#endif // !SCENERENDERING_H