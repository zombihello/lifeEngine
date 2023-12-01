#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "Render/SceneRendering.h"
#include "Render/SceneRenderTargets.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/DrawingPolicy.h"
#include "Render/LightGeometry.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/Shaders/LightingShader.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/DepthOnlyShader.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"

/**
 * @ingroup Engine
 * @brief Class of utils for lighting drawing policy
 */
class CLightingDrawingPolicyUtils
{
public:
	/**
	 * Draw point lights
	 * 
	 * @param InDeviceContextRHI		RHI device context
	 * @param InVertexFactory			Vertex factory
	 * @param InLightingVertexShader	Lighting vertex shader
	 * @param InLights					List of point lights
	 * @param InSceneView				Scene view
	 */
	template<class TShaderClass>
	static void DrawPointLights( class CBaseDeviceContextRHI* InDeviceContextRHI, CVertexFactory* InVertexFactory, TShaderClass* InLightingVertexShader, const std::list<CPointLightComponent*>* InLights, const class CSceneView& InSceneView )
	{
		// If vertex factory not support instancig - draw without it
		if ( !InVertexFactory->SupportsInstancing() )
		{
			Sys_Errorf( TEXT( "Not implemented" ) );
		}
		// Else we draw geometry with help instancing
		else
		{
			InLightingVertexShader->SetMesh( InDeviceContextRHI, *InLights, InVertexFactory, &InSceneView, InLights->size() );
			g_RHI->CommitConstants( InDeviceContextRHI );

			IndexBufferRHIRef_t		indexBufferRHI = GLightSphereMesh.GetIndexBufferRHI();
			if ( indexBufferRHI )
			{
				g_RHI->DrawIndexedPrimitive( InDeviceContextRHI, indexBufferRHI, PT_TriangleList, 0, 0, GLightSphereMesh.GetNumPrimitives(), InLights->size() );
			}
			else
			{
				g_RHI->DrawPrimitive( InDeviceContextRHI, PT_TriangleList, 0, GLightSphereMesh.GetNumPrimitives(), InLights->size() );
			}
		}
	}

	/**
	 * Draw spot lights
	 *
	 * @param InDeviceContextRHI		RHI device context
	 * @param InVertexFactory			Vertex factory
	 * @param InLightingVertexShader	Lighting vertex shader
	 * @param InLights					List of spot lights
	 * @param InSceneView				Scene view
	 */
	template<class TShaderClass>
	static void DrawSpotLights( class CBaseDeviceContextRHI* InDeviceContextRHI, CVertexFactory* InVertexFactory, TShaderClass* InLightingVertexShader, const std::list<CSpotLightComponent*>* InLights, const class CSceneView& InSceneView )
	{
		// If vertex factory not support instancig - draw without it
		if ( !InVertexFactory->SupportsInstancing() )
		{
			Sys_Errorf( TEXT( "Not implemented" ) );
		}
		// Else we draw geometry with help instancing
		else
		{
			InLightingVertexShader->SetMesh( InDeviceContextRHI, *InLights, InVertexFactory, &InSceneView, InLights->size() );
			g_RHI->CommitConstants( InDeviceContextRHI );

			IndexBufferRHIRef_t		indexBufferRHI = GLightConeMesh.GetIndexBufferRHI();
			if ( indexBufferRHI )
			{
				g_RHI->DrawIndexedPrimitive( InDeviceContextRHI, indexBufferRHI, PT_TriangleList, 0, 0, GLightConeMesh.GetNumPrimitives(), InLights->size() );
			}
			else
			{
				g_RHI->DrawPrimitive( InDeviceContextRHI, PT_TriangleList, 0, GLightConeMesh.GetNumPrimitives(), InLights->size() );
			}
		}
	}

	/**
	 * Draw directional lights
	 *
	 * @param InDeviceContextRHI		RHI device context
	 * @param InVertexFactory			Vertex factory
	 * @param InLightingVertexShader	Lighting vertex shader
	 * @param InLights					List of directionl lights
	 * @param InSceneView				Scene view
	 */
	template<class TShaderClass>
	static void DrawDirectionalLights( class CBaseDeviceContextRHI* InDeviceContextRHI, CVertexFactory* InVertexFactory, TShaderClass* InLightingVertexShader, const std::list<CDirectionalLightComponent*>* InLights, const class CSceneView& InSceneView )
	{
		// If vertex factory not support instancig - draw without it
		if ( !InVertexFactory->SupportsInstancing() )
		{
			Sys_Errorf( TEXT( "Not implemented" ) );
		}
		// Else we draw geometry with help instancing
		else
		{
			InLightingVertexShader->SetMesh( InDeviceContextRHI, *InLights, InVertexFactory, &InSceneView, InLights->size() );
			g_RHI->CommitConstants( InDeviceContextRHI );

			IndexBufferRHIRef_t		indexBufferRHI = GLightQuadMesh.GetIndexBufferRHI();
			if ( indexBufferRHI )
			{
				g_RHI->DrawIndexedPrimitive( InDeviceContextRHI, indexBufferRHI, PT_TriangleList, 0, 0, GLightQuadMesh.GetNumPrimitives(), InLights->size() );
			}
			else
			{
				g_RHI->DrawPrimitive( InDeviceContextRHI, PT_TriangleList, 0, GLightQuadMesh.GetNumPrimitives(), InLights->size() );
			}
		}
	}
};

/**
 * @ingroup Engine
 * @brief Base drawing policy of lighting
 */
class CBaseLightingDrawingPolicy : protected CMeshDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, g_Engine->GetDefaultMaterial(), InDepthBias );
	}

	/**
	 * Set shader parameters
	 *
	 * @param InDeviceContextRHI			RHI device context
	 * @param InAlbedoRoughnessGBufferRHI	RHI albedo roughness GBuffer texture
	 * @param InNormalMetalGBufferRHI		RHI normal metal GBuffer texture
	 * @param InEmissionGBufferRHI			RHI emission GBuffer texture
	 * @param InDepthBufferRHI				RHI depth buffer texture
	 */
	void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, Texture2DRHIParamRef_t InAlbedoRoughnessGBufferRHI, Texture2DRHIParamRef_t InNormalMetalGBufferRHI, Texture2DRHIParamRef_t InEmissionGBufferRHI, Texture2DRHIParamRef_t InDepthBufferRHI )
	{
		CBaseLightingPixelShader*		baseLightingPixelShader = ( CBaseLightingPixelShader* )pixelShader;
		Assert( baseLightingPixelShader );

		// Set constant parameters
		vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );
		pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );

		// Albedo Roughness GBuffer
		baseLightingPixelShader->SetAlbedoRoughnessGBufferTexture( InDeviceContextRHI, InAlbedoRoughnessGBufferRHI );
		baseLightingPixelShader->SetAlbedoRoughnessGBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Normal Metal GBuffer
		baseLightingPixelShader->SetNormalMetalGBufferTexture( InDeviceContextRHI, InNormalMetalGBufferRHI );
		baseLightingPixelShader->SetNormalMetalGBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Emission AO GBuffer
		baseLightingPixelShader->SetEmissionGBufferTexture( InDeviceContextRHI, InEmissionGBufferRHI );
		baseLightingPixelShader->SetEmissionGBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Depth buffer
		baseLightingPixelShader->SetDepthBufferTexture( InDeviceContextRHI, InDepthBufferRHI );
		baseLightingPixelShader->SetDepthBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Gamma
		baseLightingPixelShader->SetGamma( InDeviceContextRHI, g_Engine->GetGamma() );
	}

	/**
	 * Set render state for drawing
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InPassType			Pass type
	 */
	virtual void SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI ) override
	{
		CMeshDrawingPolicy::SetRenderState( InDeviceContextRHI );
		g_RHI->SetDepthState( InDeviceContextRHI, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
		g_RHI->SetStencilState( InDeviceContextRHI, TStaticStencilStateRHI<true, CF_NotEqual, SO_Keep, SO_Increment, SO_Keep, true, CF_NotEqual, SO_Keep, SO_Decrement, SO_Keep, 0xFF, 0>::GetRHI() );
		g_RHI->SetBlendState( InDeviceContextRHI, TStaticBlendStateRHI<BO_Add, BF_One, BF_One>::GetRHI() );
		g_RHI->SetColorWriteEnable( InDeviceContextRHI, true );
	}

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy
	 */
	virtual RasterizerStateRHIRef_t GetRasterizerState() const override
	{
		if ( !rasterizerState )
		{
			rasterizerState = TStaticRasterizerStateRHI<FM_Solid, CM_CCW>::GetRHI();
		}
		return rasterizerState;
	}
};

/**
 * @ingroup Engine
 * @brief Base drawing policy of stencil lighting pass
 */
class CBaseStencilLightingDrawingPolicy : protected CMeshDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, g_Engine->GetDefaultMaterial(), InDepthBias );
	}

	/**
	 * Set shader parameters
	 *
	 * @param InDeviceContextRHI			RHI device context
	 * @param InAlbedoRoughnessGBufferRHI	RHI albedo roughness GBuffer texture
	 * @param InNormalMetalGBufferRHI		RHI normal metal GBuffer texture
	 * @param InEmissionGBufferRHI			RHI emission GBuffer texture
	 * @param InDepthBufferRHI				RHI depth buffer texture
	 */
	virtual void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI ) override
	{
		// Set constant parameters
		vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );
		pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );
	}

	/**
	 * Set render state for drawing
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InPassType			Pass type
	 */
	virtual void SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI ) override
	{
		CMeshDrawingPolicy::SetRenderState( InDeviceContextRHI );
		g_RHI->SetDepthState( InDeviceContextRHI, TStaticDepthStateRHI<false>::GetRHI() );
		g_RHI->SetStencilState( InDeviceContextRHI, TStaticStencilStateRHI<true, CF_Always, SO_Keep, SO_Increment, SO_Keep, true, CF_Always, SO_Keep, SO_Decrement, SO_Keep>::GetRHI() );
		g_RHI->SetBlendState( InDeviceContextRHI, TStaticBlendStateRHI<>::GetRHI() );
		g_RHI->SetColorWriteEnable( InDeviceContextRHI, false );
		InDeviceContextRHI->ClearDepthStencil( g_SceneRenderTargets.GetSceneDepthZSurface(), false, true );
	}

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy
	 */
	virtual RasterizerStateRHIRef_t GetRasterizerState() const override
	{
		if ( !rasterizerState )
		{
			rasterizerState = TStaticRasterizerStateRHI<FM_Solid, CM_CCW>::GetRHI();
		}
		return rasterizerState;
	}
};

/**
 * @ingroup Engine
 * @brief Empty template class of lighting drawing policy (don't use!)
 */
template<ELightType InType>
class TLightingDrawingPolicy : public CBaseLightingDrawingPolicy
{
	static_assert( InType != LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Point lighting drawing policy
 */
template<>
class TLightingDrawingPolicy<LT_Point> : public CBaseLightingDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InLights			List of point lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( const std::list<CPointLightComponent*>* InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( GLightSphereMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering	
		uint64					vertexFactoryHash = vertexFactory->GetType()->GetHash();
		vertexShader			= lightingVertexShader	= g_ShaderManager->FindInstance<TLightingVertexShader<LT_Point>>( vertexFactoryHash );
		pixelShader				= lightingPixelShader	= g_ShaderManager->FindInstance<TLightingPixelShader<LT_Point>>( vertexFactoryHash );
		pointLightComponents	= InLights;
	}

	/**
	 * Draw light
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSceneView			Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CSceneView& InSceneView )
	{
		CLightingDrawingPolicyUtils::DrawPointLights( InDeviceContextRHI, vertexFactory, lightingVertexShader, pointLightComponents, InSceneView );
	}

private:
	TLightingVertexShader<LT_Point>*				lightingVertexShader;		/**< Point light vertex shader */
	TLightingPixelShader<LT_Point>*					lightingPixelShader;		/**< Point light pixel shader */
	const std::list<CPointLightComponent*>*			pointLightComponents;		/**< List of point light components */
};

/**
 * @ingroup Engine
 * @brief Spot lighting drawing policy
 */
template<>
class TLightingDrawingPolicy<LT_Spot> : public CBaseLightingDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InLights			List of spot lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( const std::list<CSpotLightComponent*>* InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( GLightConeMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering
		uint64					vertexFactoryHash		= vertexFactory->GetType()->GetHash();
		vertexShader			= lightingVertexShader	= g_ShaderManager->FindInstance<TLightingVertexShader<LT_Spot>>( vertexFactoryHash );
		pixelShader				= lightingPixelShader	= g_ShaderManager->FindInstance<TLightingPixelShader<LT_Spot>>( vertexFactoryHash );
		spotLightComponents		= InLights;
	}

	/**
	 * Draw light
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSceneView			Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CSceneView& InSceneView )
	{
		CLightingDrawingPolicyUtils::DrawSpotLights( InDeviceContextRHI, vertexFactory, lightingVertexShader, spotLightComponents, InSceneView );
	}

private:
	TLightingVertexShader<LT_Spot>*				lightingVertexShader;		/**< Spot light vertex shader */
	TLightingPixelShader<LT_Spot>*				lightingPixelShader;		/**< Spot light pixel shader */
	const std::list<CSpotLightComponent*>*		spotLightComponents;		/**< List of spot light components */
};

/**
 * @ingroup Engine
 * @brief Directional lighting drawing policy
 */
template<>
class TLightingDrawingPolicy<LT_Directional> : public CBaseLightingDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InLights			List of directional lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( const std::list<CDirectionalLightComponent*>* InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( GLightQuadMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering
		uint64						vertexFactoryHash		= vertexFactory->GetType()->GetHash();
		vertexShader				= lightingVertexShader	= g_ShaderManager->FindInstance<TLightingVertexShader<LT_Directional>>( vertexFactoryHash );
		pixelShader					= lightingPixelShader	= g_ShaderManager->FindInstance<TLightingPixelShader<LT_Directional>>( vertexFactoryHash );
		directionalLightComponents	= InLights;
	}

	/**
	 * Set render state for drawing
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InPassType			Pass type
	 */
	virtual void SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI ) override
	{
		CMeshDrawingPolicy::SetRenderState( InDeviceContextRHI );
		g_RHI->SetDepthState( InDeviceContextRHI, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
		g_RHI->SetStencilState( InDeviceContextRHI, TStaticStencilStateRHI<>::GetRHI() );
		g_RHI->SetBlendState( InDeviceContextRHI, TStaticBlendStateRHI<BO_Add, BF_One, BF_One>::GetRHI() );
		g_RHI->SetColorWriteEnable( InDeviceContextRHI, true );
	}

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy
	 */
	virtual RasterizerStateRHIRef_t GetRasterizerState() const override
	{
		if ( !rasterizerState )
		{
			rasterizerState = TStaticRasterizerStateRHI<FM_Solid, CM_CCW>::GetRHI();
		}
		return rasterizerState;
	}

	/**
	 * Draw light
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InSceneView Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CSceneView& InSceneView )
	{
		CLightingDrawingPolicyUtils::DrawDirectionalLights( InDeviceContextRHI, vertexFactory, lightingVertexShader, directionalLightComponents, InSceneView );
	}

private:
	TLightingVertexShader<LT_Directional>*				lightingVertexShader;			/**< Directional light vertex shader */
	TLightingPixelShader<LT_Directional>*				lightingPixelShader;			/**< Directional light pixel shader */
	const std::list<CDirectionalLightComponent*>*		directionalLightComponents;		/**< List of directional light components */
};

/**
 * @ingroup Engine
 * @brief Empty template class of stencil lighting pass drawing policy (don't use!)
 */
template<ELightType InType>
class TStencilLightingDrawingPolicy : public CBaseStencilLightingDrawingPolicy
{
	static_assert( InType != LT_Unknown, "This template class cannot be used" );
};

/**
 * @ingroup Engine
 * @brief Stencil point lighting pass drawing policy
 */
template<>
class TStencilLightingDrawingPolicy<LT_Point> : public CBaseStencilLightingDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InLights			List of point lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( const std::list<CPointLightComponent*>* InLights, float InDepthBias = 0.f )
	{
		CBaseStencilLightingDrawingPolicy::Init( GLightSphereMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering	
		uint64					vertexFactoryHash = vertexFactory->GetType()->GetHash();
		vertexShader			= lightingVertexShader = g_ShaderManager->FindInstance<TDepthOnlyLightingVertexShader<LT_Point>>( vertexFactoryHash );
		pixelShader				= g_ShaderManager->FindInstance<CDepthOnlyPixelShader>( vertexFactoryHash );
		pointLightComponents	= InLights;
	}

	/**
	 * Draw light
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InSceneView Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CSceneView& InSceneView )
	{
		CLightingDrawingPolicyUtils::DrawPointLights( InDeviceContextRHI, vertexFactory, lightingVertexShader, pointLightComponents, InSceneView );
	}

private:
	TDepthOnlyLightingVertexShader<LT_Point>*		lightingVertexShader;		/**< Depth only point light vertex shader */
	const std::list<CPointLightComponent*>*			pointLightComponents;		/**< List of point light components */
};

/**
 * @ingroup Engine
 * @brief Stencil spot lighting pass drawing policy
 */
template<>
class TStencilLightingDrawingPolicy<LT_Spot> : public CBaseStencilLightingDrawingPolicy
{
public:
	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InLights			List of spot lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( const std::list<CSpotLightComponent*>* InLights, float InDepthBias = 0.f )
	{
		CBaseStencilLightingDrawingPolicy::Init( GLightConeMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering
		uint64			vertexFactoryHash = vertexFactory->GetType()->GetHash();
		vertexShader	= lightingVertexShader = g_ShaderManager->FindInstance<TDepthOnlyLightingVertexShader<LT_Spot>>( vertexFactoryHash );
		pixelShader		= g_ShaderManager->FindInstance<CDepthOnlyPixelShader>( vertexFactoryHash );
		spotLightComponents = InLights;
	}

	/**
	 * Draw light
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSceneView			Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CSceneView& InSceneView )
	{
		CLightingDrawingPolicyUtils::DrawSpotLights( InDeviceContextRHI, vertexFactory, lightingVertexShader, spotLightComponents, InSceneView );
	}

private:
	TDepthOnlyLightingVertexShader<LT_Spot>*	lightingVertexShader;		/**< Depth only spot light vertex shader */
	const std::list<CSpotLightComponent*>*		spotLightComponents;		/**< List of spot light components */
};


/*
==================
CSceneRenderer::RenderLights
==================
*/
void CSceneRenderer::RenderLights( class CBaseDeviceContextRHI* InDeviceContext )
{
	if ( !scene )
	{
		return;
	}

	SCOPED_DRAW_EVENT( EventLightPass, DEC_LIGHT, TEXT( "LightPass" ) );

	// Copy scene depth buffer to light pass depth for using it's data in RT and in shaders (reconstruction world position)
	g_SceneRenderTargets.ResolveLightPassDepth( InDeviceContext );

	// Begin rendering light pass
	g_SceneRenderTargets.BeginRenderingSceneColorHDR( InDeviceContext );
	InDeviceContext->ClearSurface( g_SceneRenderTargets.GetSceneColorHDRSurface(), sceneView->GetBackgroundColor() );

	std::list<CPointLightComponent*>			pointLightComponents;
	std::list<CSpotLightComponent*>			spotLightComponents;
	std::list<CDirectionalLightComponent*>		directionalLightComponents;

	// Separating light components by type
	{
		const std::list<CLightComponent*>&				lightComponents = scene->GetVisibleLights();
		for ( auto it = lightComponents.begin(), itEnd = lightComponents.end(); it != itEnd; ++it )
		{
			CLightComponent*		lightComponent = *it;
			switch ( lightComponent->GetLightType() )
			{
			case LT_Point:			pointLightComponents.push_back( ( CPointLightComponent* )lightComponent );		break;
			case LT_Spot:			spotLightComponents.push_back( ( CSpotLightComponent* )lightComponent );		break;
			case LT_Directional:	directionalLightComponents.push_back( ( CDirectionalLightComponent* )lightComponent );	break;
			default:
				Warnf( TEXT( "Unknown light type in '%s'\n" ), lightComponent->GetName() );
				break;
			}
		}
	}

	// Render point lights
	if ( !pointLightComponents.empty() )
	{
		TStencilLightingDrawingPolicy<LT_Point>	stencilLightingDrawingPolicy;
		TLightingDrawingPolicy<LT_Point>		lightingDrawingPolicy;
		stencilLightingDrawingPolicy.Init( &pointLightComponents );
		lightingDrawingPolicy.Init( &pointLightComponents );
		
		// Stencil pass
		stencilLightingDrawingPolicy.SetRenderState( InDeviceContext );
		stencilLightingDrawingPolicy.SetShaderParameters( InDeviceContext );
		stencilLightingDrawingPolicy.Draw( InDeviceContext, *sceneView );

		// Base pass
		lightingDrawingPolicy.SetRenderState( InDeviceContext );
		lightingDrawingPolicy.SetShaderParameters( InDeviceContext, g_SceneRenderTargets.GetAlbedo_Roughness_GBufferTexture(), g_SceneRenderTargets.GetNormal_Metal_GBufferTexture(), g_SceneRenderTargets.GetEmission_AO_GBufferTexture(), g_SceneRenderTargets.GetLightPassDepthZTexture() );
		lightingDrawingPolicy.Draw( InDeviceContext, *sceneView );
	}

	// Render spot lights
	if ( !spotLightComponents.empty() )
	{
		TStencilLightingDrawingPolicy<LT_Spot>	stencilLightingDrawingPolicy;
		TLightingDrawingPolicy<LT_Spot>			lightingDrawingPolicy;
		stencilLightingDrawingPolicy.Init( &spotLightComponents );
		lightingDrawingPolicy.Init( &spotLightComponents );

		// Stencil pass
		stencilLightingDrawingPolicy.SetRenderState( InDeviceContext );
		stencilLightingDrawingPolicy.SetShaderParameters( InDeviceContext );
		stencilLightingDrawingPolicy.Draw( InDeviceContext, *sceneView );

		// Base pass
		lightingDrawingPolicy.SetRenderState( InDeviceContext );
		lightingDrawingPolicy.SetShaderParameters( InDeviceContext, g_SceneRenderTargets.GetAlbedo_Roughness_GBufferTexture(), g_SceneRenderTargets.GetNormal_Metal_GBufferTexture(), g_SceneRenderTargets.GetEmission_AO_GBufferTexture(), g_SceneRenderTargets.GetLightPassDepthZTexture() );
		lightingDrawingPolicy.Draw( InDeviceContext, *sceneView );
	}

	// Render directional lights
	if ( !directionalLightComponents.empty() )
	{
		TLightingDrawingPolicy<LT_Directional>			lightingDrawingPolicy;
		lightingDrawingPolicy.Init( &directionalLightComponents );

		// Base pass
		lightingDrawingPolicy.SetRenderState( InDeviceContext );
		lightingDrawingPolicy.SetShaderParameters( InDeviceContext, g_SceneRenderTargets.GetAlbedo_Roughness_GBufferTexture(), g_SceneRenderTargets.GetNormal_Metal_GBufferTexture(), g_SceneRenderTargets.GetEmission_AO_GBufferTexture(), g_SceneRenderTargets.GetLightPassDepthZTexture() );
		lightingDrawingPolicy.Draw( InDeviceContext, *sceneView );
	}

	// Finish rendering light pass
	g_RHI->SetStencilState( InDeviceContext, TStaticStencilStateRHI<>::GetRHI() );
	g_SceneRenderTargets.FinishRenderingSceneColorHDR( InDeviceContext );
}