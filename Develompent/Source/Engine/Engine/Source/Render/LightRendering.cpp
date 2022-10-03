#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "Render/SceneRendering.h"
#include "Render/SceneRenderTargets.h"
#include "Render/SceneUtils.h"
#include "Render/Scene.h"
#include "Render/DrawingPolicy.h"
#include "Render/Sphere.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseDeviceContextRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/StaticStatesRHI.h"
#include "RHI/TypesRHI.h"
#include "Render/Shaders/LightingShader.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/DirectionalLightComponent.h"

/**
 * @ingroup Engine
 * @brief Base drawing policy of lighting
 */
class CBaseLightingDrawingPolicy : protected CMeshDrawingPolicy
{
public:
	/**
	 * @brief Enumeration pass types of drawing light
	 */
	enum EPassType
	{
		PT_Stencil,		/**< Stencil pass */
		PT_Base			/**< Base pass */
	};

	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, GEngine->GetDefaultMaterial(), InDepthBias );
	}

	/**
	 * Set shader parameters
	 *
	 * @param InDeviceContextRHI			RHI device context
	 * @param InDiffuseRoughnessGBufferRHI	RHI diffuse roughness GBuffer texture
	 * @param InNormalMetalGBufferRHI		RHI normal metal GBuffer texture
	 * @param InDepthBufferRHI				Rhi depth buffer texture
	 */
	void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, Texture2DRHIParamRef_t InDiffuseRoughnessGBufferRHI, Texture2DRHIParamRef_t InNormalMetalGBufferRHI, Texture2DRHIParamRef_t InDepthBufferRHI )
	{
		CBaseLightingPixelShader*		baseLightingPixelShader = ( CBaseLightingPixelShader* )pixelShader;
		check( baseLightingPixelShader );

		// Set constant parameters
		vertexShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );
		pixelShader->SetConstantParameters( InDeviceContextRHI, vertexFactory, nullptr );

		// Diffuse Roughness GBuffer
		baseLightingPixelShader->SetDiffuseRoughnessGBufferTexture( InDeviceContextRHI, InDiffuseRoughnessGBufferRHI );
		baseLightingPixelShader->SetDiffuseRoughnessGBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Normal Metal GBuffer
		baseLightingPixelShader->SetNormalMetalGBufferTexture( InDeviceContextRHI, InNormalMetalGBufferRHI );
		baseLightingPixelShader->SetNormalMetalGBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );

		// Depth buffer
		baseLightingPixelShader->SetDepthBufferTexture( InDeviceContextRHI, InDepthBufferRHI );
		baseLightingPixelShader->SetDepthBufferSamplerState( InDeviceContextRHI, TStaticSamplerStateRHI<>::GetRHI() );
	}

	/**
	 * Set render state for drawing
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InPassType			Pass type
	 */
	void SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI, EPassType InPassType )
	{
		CMeshDrawingPolicy::SetRenderState( InDeviceContextRHI );

		switch ( InPassType )
		{
		case PT_Base:
			GRHI->SetDepthTest( InDeviceContextRHI, TStaticDepthStateRHI<false, CF_Always>::GetRHI() );
			GRHI->SetBlendState( InDeviceContextRHI, TStaticBlendState<BO_Add, BF_One, BF_SourceColor>::GetRHI() );
			break;

		case PT_Stencil:
			GRHI->SetDepthTest( InDeviceContextRHI, TStaticDepthStateRHI<false, CF_LessEqual>::GetRHI() );
			break;

		default:
			appErrorf( TEXT( "Unknown pass type 0x%X" ), InPassType );
			break;
		}
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
	FORCEINLINE void Init( const std::list<TRefCountPtr<CPointLightComponent>>& InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( GLightSphereMesh.GetVertexFactory(), InDepthBias );

		// Override shaders for lighting rendering
		uint64					vertexFactoryHash		= vertexFactory->GetType()->GetHash();
		vertexShader			= lightingVertexShader	= GShaderManager->FindInstance<TLightingVertexShader<LT_Point>>( vertexFactoryHash );
		pixelShader				= lightingPixelShader	= GShaderManager->FindInstance<TLightingPixelShader<LT_Point>>( vertexFactoryHash );
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
		// If vertex factory not support instancig - draw without it
		if ( !vertexFactory->SupportsInstancing() )
		{
			appErrorf( TEXT( "Not implemented" ) );
		}
		// Else we draw geometry with help instancing
		else
		{
			IndexBufferRHIRef_t		indexBufferRHI = GLightSphereMesh.GetIndexBufferRHI();
			lightingVertexShader->SetMesh( InDeviceContextRHI, pointLightComponents, vertexFactory, &InSceneView, pointLightComponents.size() );
			GRHI->CommitConstants( InDeviceContextRHI );

			if ( indexBufferRHI )
			{
				GRHI->DrawIndexedPrimitive( InDeviceContextRHI, indexBufferRHI, PT_TriangleList, 0, 0, GLightSphereMesh.GetNumPrimitives(), pointLightComponents.size() );
			}
			else
			{
				GRHI->DrawPrimitive( InDeviceContextRHI, PT_TriangleList, 0, GLightSphereMesh.GetNumPrimitives(), pointLightComponents.size() );
			}
		}
	}

private:
	TLightingVertexShader<LT_Point>*					lightingVertexShader;		/**< Point light vertex shader */
	TLightingPixelShader<LT_Point>*						lightingPixelShader;		/**< Point light pixel shader */
	std::list<TRefCountPtr<CPointLightComponent>>		pointLightComponents;		/**< List of point light components */
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
	 * @param InVertexFactory	Vertex factory
	 * @param InLights			List of spot lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( InVertexFactory, InDepthBias );

		// Override shaders for lighting rendering
		uint64					vertexFactoryHash		= InVertexFactory->GetType()->GetHash();
		vertexShader			= lightingVertexShader	= GShaderManager->FindInstance<TLightingVertexShader<LT_Spot>>( vertexFactoryHash );
		pixelShader				= lightingPixelShader	= GShaderManager->FindInstance<TLightingPixelShader<LT_Spot>>( vertexFactoryHash );
		spotLightComponents		= InLights;
	}

private:
	TLightingVertexShader<LT_Spot>*					lightingVertexShader;		/**< Spot light vertex shader */
	TLightingPixelShader<LT_Spot>*					lightingPixelShader;		/**< Spot light pixel shader */
	std::list<TRefCountPtr<CSpotLightComponent>>	spotLightComponents;		/**< List of spot light components */
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
	 * @param InVertexFactory	Vertex factory
	 * @param InLights			List of directional lights
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, float InDepthBias = 0.f )
	{
		CBaseLightingDrawingPolicy::Init( InVertexFactory, InDepthBias );

		// Override shaders for lighting rendering
		uint64						vertexFactoryHash		= InVertexFactory->GetType()->GetHash();
		vertexShader				= lightingVertexShader	= GShaderManager->FindInstance<TLightingVertexShader<LT_Directional>>( vertexFactoryHash );
		pixelShader					= lightingPixelShader	= GShaderManager->FindInstance<TLightingPixelShader<LT_Directional>>( vertexFactoryHash );
		directionalLightComponents	= InLights;
	}

private:
	TLightingVertexShader<LT_Directional>*				lightingVertexShader;			/**< Directional light vertex shader */
	TLightingPixelShader<LT_Directional>*				lightingPixelShader;			/**< Directional light pixel shader */
	std::list<TRefCountPtr<CDirectionalLightComponent>>	directionalLightComponents;		/**< List of directional light components */
};

void CSceneRenderer::RenderLights( class CBaseDeviceContextRHI* InDeviceContext )
{
	if ( !scene )
	{
		return;
	}

	// Copy scene depth buffer to light attenuation depth for using him's data in RT and in shaders (reconstruction world position)
	GSceneRenderTargets.ResolveLightAttenuationDepth( InDeviceContext );

	// Begin rendering light attenuation
	GSceneRenderTargets.FinishRenderingGBuffer( InDeviceContext );
	GSceneRenderTargets.BeginRenderingLightAttenuation( InDeviceContext );
	InDeviceContext->ClearSurface( GSceneRenderTargets.GetSceneColorSurface(), sceneView->GetBackgroundColor() );

	std::list<TRefCountPtr<CPointLightComponent>>			pointLightComponents;
	std::list<TRefCountPtr<CSpotLightComponent>>			spotLightComponents;
	std::list<TRefCountPtr<CDirectionalLightComponent>>		directionalLightComponents;

	// Separating light components by type
	{
		const std::list<LightComponentRef_t>&				lightComponents = scene->GetVisibleLights();
		for ( auto it = lightComponents.begin(), itEnd = lightComponents.end(); it != itEnd; ++it )
		{
			LightComponentRef_t		lightComponent = *it;
			switch ( lightComponent->GetLightType() )
			{
			case LT_Point:			pointLightComponents.push_back( lightComponent );		break;
			case LT_Spot:			spotLightComponents.push_back( lightComponent );		break;
			case LT_Directional:	directionalLightComponents.push_back( lightComponent );	break;
			default:
				LE_LOG( LT_Warning, LC_Render, TEXT( "Unknown light type in '%s'" ), lightComponent->GetName() );
				break;
			}
		}
	}

	// Render point lights
	{
		TLightingDrawingPolicy<LT_Point>		lightingDrawingPolicy;
		lightingDrawingPolicy.Init( pointLightComponents );
		lightingDrawingPolicy.SetShaderParameters( InDeviceContext, GSceneRenderTargets.GetDiffuse_Roughness_GBufferTexture(), GSceneRenderTargets.GetNormal_Metal_GBufferTexture(), GSceneRenderTargets.GetLightAttenuationDepthZTexture() );
		lightingDrawingPolicy.SetRenderState( InDeviceContext, TLightingDrawingPolicy<LT_Point>::PT_Base );
		lightingDrawingPolicy.Draw( InDeviceContext, *sceneView );
	}

	GSceneRenderTargets.FinishRenderingLightAttenuation( InDeviceContext );
}