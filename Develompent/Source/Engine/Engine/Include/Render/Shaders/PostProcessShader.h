/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef POSTPROCESSHADER_H
#define POSTPROCESSHADER_H

#include "Shader.h"
#include "ShaderManager.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

/**
 * @ingroup Engine
 * @brief Class of pixel shader for post process
 */
template<bool InApplyHDR = false, bool InApplyGammaCorrection = false>
class TPostProcessPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( TPostProcessPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem ) override
	{
		CShader::Init( InShaderCacheItem );
		sceneColorParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "sceneColor" ) );
		sceneColorSamplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "sceneColorSampler" ) );
		bloomParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "bloom" ) );
		bloomSamplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "bloomSampler" ) );
		hdrExposureParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "hdrExposure" ), !InApplyHDR );
		gammaParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "gamma" ), !InApplyGammaCorrection );
	}

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		if ( !InVFMetaType )
		{
			return true;
		}

		return InVFMetaType->GetHash() == CSimpleElementVertexFactory::staticType.GetHash();
	}

	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment )
	{
		CShader::ModifyCompilationEnvironment( InShaderPlatform, InEnvironment );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "APPLY_HDR" ), InApplyHDR ? TEXT( "1" ) : TEXT( "0" ) ) );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "APPLY_GAMMA_CORRECTION" ), InApplyGammaCorrection ? TEXT( "1" ) : TEXT( "0" ) ) );
	}
#endif // WITH_EDITOR

	/**
	 * @brief Set texture parameter
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, sceneColorParameter, InTextureRHI );
	}

	/**
	 * @brief Set sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, sceneColorSamplerStateParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set bloom texture parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetBloomTexture( class CBaseDeviceContextRHI* InDeviceContextRHI, const Texture2DRHIParamRef_t InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, bloomParameter, InTextureRHI );
	}

	/**
	 * @brief Set bloom sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetBloomSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, bloomSamplerStateParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set exposure
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InExposure			New Exposure
	 */
	FORCEINLINE void SetExposure( class CBaseDeviceContextRHI* InDeviceContextRHI, float InExposure )
	{
		if ( InApplyHDR )
		{
			SetPixelShaderValue( InDeviceContextRHI, hdrExposureParameter, InExposure );
		}
	}

	/**
	 * @brief Set gamma
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InGamma				gamma
	 */
	FORCEINLINE void SetGamma( class CBaseDeviceContextRHI* InDeviceContextRHI, float InGamma )
	{
		if ( InApplyGammaCorrection )
		{
			SetPixelShaderValue( InDeviceContextRHI, gammaParameter, InGamma );
		}
	}

private:
	CShaderResourceParameter			sceneColorParameter;					/**< Scene color texture parameter */
	CShaderResourceParameter			sceneColorSamplerStateParameter;		/**< Scene color sampler sate parameter */
	CShaderResourceParameter			bloomParameter;							/**< Bloom texture parameter */
	CShaderResourceParameter			bloomSamplerStateParameter;				/**< Bloom sampler sate parameter */
	CShaderParameter					hdrExposureParameter;					/**< HDR exposure parameter */
	CShaderParameter					gammaParameter;							/**< Gamma parameter */
};

// #define avoids a lot of code duplication
#define VARIATION_PIXEL_SHADER_TYPE( HDRExposure, GammaCorrection ) \
	typedef TPostProcessPixelShader<HDRExposure, GammaCorrection>	TPostProcessPixelShader_##HDRExposure##GammaCorrection; \
	IMPLEMENT_SHADER_TYPE( template<>, TPostProcessPixelShader_##HDRExposure##GammaCorrection, TEXT( "PostProcessPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true )

VARIATION_PIXEL_SHADER_TYPE( false, false );
VARIATION_PIXEL_SHADER_TYPE( true, true );
VARIATION_PIXEL_SHADER_TYPE( true, false );
VARIATION_PIXEL_SHADER_TYPE( false, true );
#undef VARIATION_PIXEL_SHADER_TYPE

#endif // !POSTPROCESSHADER_H

