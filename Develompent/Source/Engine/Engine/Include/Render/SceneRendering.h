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
#include "LEBuild.h"

#if !SHIPPING_BUILD
#include "Render/Shaders/WireframeShader.h"
#endif // !SHIPPING_BUILD

/**
 * @ingroup Engine
 * Draw policy of static mesh
 */
class CStaticMeshDrawPolicy : public CMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 */
	CStaticMeshDrawPolicy();

	/**
	 * Set shader parameters
	 *
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI ) override;
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
		: wireframeColor( CColor::red )
	{}

	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InWireframeColor	Wireframe color
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, const CColor& InWireframeColor = CColor::red, float InDepthBias = 0.f )
	{
		TAssetHandle<CMaterial>		wireframeMaterial = GEngine->GetDefaultWireframeMaterial();
		TSharedPtr<CMaterial>		wireframeMaterialRef = wireframeMaterial.ToSharedPtr();
		if ( !wireframeMaterialRef )
		{
			return;
		}

		wireframeColor		= InWireframeColor;	
		wireframeMaterialRef->SetVectorParameterValue( TEXT( "wireframeColor" ), InWireframeColor.ToNormalizedVector4D() );		// TODO: For correct work need implement material instances
		
		InitInternal( InVertexFactory, wireframeMaterial, InDepthBias );

		// Override shaders for wireframe rendering
		uint64			vertexFactoryHash = InVertexFactory->GetType()->GetHash();
		vertexShader	= GShaderManager->FindInstance<CWireframeVertexShader>( vertexFactoryHash );
		pixelShader		= GShaderManager->FindInstance<CWireframePixelShader>( vertexFactoryHash );
	}

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy
	 */
	virtual RasterizerStateRHIRef_t GetRasterizerState() const
	{
		if ( !rasterizerState )
		{
			const SRasterizerStateInitializerRHI		initializer =
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
	CColor			wireframeColor;		/**< Wireframe color */
};
#endif // !SHIPPING_BUILD

/**
 * @ingroup Engine
 * Class for render scene
 */
class CSceneRenderer
{
public:
	/**
	 * Constructor
	 * 
	 * @param InSceneView	Scene view
	 * @param InScene		Scene
	 */
	CSceneRenderer( CSceneView* InSceneView, class CScene* InScene = nullptr );

	/**
	 * Begin render view target
	 * @param InViewportRHI		Viewport RHI
	 */
	void BeginRenderViewTarget( ViewportRHIParamRef_t InViewportRHI );

	/**
	 * Render scene
	 * 
	 * @param InViewportRHI		Viewport RHI
	 */
	void Render( ViewportRHIParamRef_t InViewportRHI );

#if ENABLE_HITPROXY
	/**
	 * Render hit proxies
	 * @param InViewportRHI		Viewport RHI
	 */
	void RenderHitProxies( ViewportRHIParamRef_t InViewportRHI );
#endif // ENABLE_HITPROXY

	/**
	 * Finish rendering a view, writing the contents to backbuffer of InViewportRHI
	 * @param InViewportRHI		Viewport RHI
	 */
	void FinishRenderViewTarget( ViewportRHIParamRef_t InViewportRHI );

private:
	class CScene*	scene;			/**< Scene */
	CSceneView*		sceneView;		/**< Scene view */
};

#endif // !SCENERENDERING_H