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

#if !SHIPPING_BUILD
#include "Render/Shaders/WireframeShader.h"
#endif // !SHIPPING_BUILD

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
	 * Set shader parameters
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI ) override;
};

#if !SHIPPING_BUILD
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

		// Override shaders for wireframe rendering
		uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
		vertexShader	= GShaderManager->FindInstance<FWireframeVertexShader>( vertexFactoryHash );
		pixelShader		= GShaderManager->FindInstance<FWireframePixelShader>( vertexFactoryHash );
	}

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy
	 */
	virtual FRasterizerStateRHIRef GetRasterizerState() const
	{
		if ( !rasterizerState )
		{
			const FRasterizerStateInitializerRHI		initializer =
			{
				FM_Wireframe,
				CM_None,
				depthBias,
				0.f,
				true
			};
			rasterizerState = GRHI->CreateRasterizerState( initializer );
		}

		return rasterizerState;
	}

	/**
	 * @brief Get type hash
	 * @return Return hash of drawing policy
	 */
	virtual uint64 GetTypeHash() const override
	{
		return appMemFastHash( wireframeColor, TBaseMeshDrawingPolicy::GetTypeHash() );
	}

private:
	FColor			wireframeColor;		/**< Wireframe color */
};
#endif // !SHIPPING_BUILD

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
	 * @param InSceneView	Scene view
	 * @param InScene		Scene
	 */
	FSceneRenderer( FSceneView* InSceneView, class FScene* InScene = nullptr );

	/**
	 * Begin render view target
	 * @param InViewportRHI		Viewport RHI
	 */
	void BeginRenderViewTarget( FViewportRHIParamRef InViewportRHI );

	/**
	 * Render scene
	 * 
	 * @param InViewportRHI		Viewport RHI
	 */
	void Render( FViewportRHIParamRef InViewportRHI );

	/**
	 * Finish rendering a view, writing the contents to backbuffer of InViewportRHI
	 * @param InViewportRHI		Viewport RHI
	 */
	void FinishRenderViewTarget( FViewportRHIParamRef InViewportRHI );

private:
	class FScene*	scene;			/**< Scene */
	FSceneView*		sceneView;		/**< Scene view */
};

#endif // !SCENERENDERING_H