/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GAUSSIANBLURSHADER_H
#define GAUSSIANBLURSHADER_H

#include "Shader.h"
#include "ShaderManager.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

/**
 * @ingroup Engine
 * @brief Gaussian blur pixel shader
 */
template<bool InHorizontalBlur = false>
class TGaussianBlurPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( TGaussianBlurPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param InShaderCacheItem		Cache of shader
	 */
	virtual void Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem ) override
	{
		CShader::Init( InShaderCacheItem );
		bufferParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "buffer" ) );
		bufferSamplerStateParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "bufferSampler" ) );
		texOffsetParameter.Bind( InShaderCacheItem.parameterMap, TEXT( "texOffset" ) );
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
	 * @param InShaderPlatform	Shader platform
	 * @param InEnvironment		Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment )
	{
		CShader::ModifyCompilationEnvironment( InShaderPlatform, InEnvironment );
		InEnvironment.difinitions.insert( std::make_pair( TEXT( "HORIZONTAL_BLUR" ), InHorizontalBlur ? TEXT( "1" ) : TEXT( "0" ) ) );
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
		SetTextureParameter( InDeviceContextRHI, bufferParameter, InTextureRHI );
	}

	/**
	 * @brief Set sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, bufferSamplerStateParameter, InSamplerStateRHI );
	}

	/**
	 * @brief Set texture size
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureSize			New texture size
	 */
	FORCEINLINE void SetTextureSize( class CBaseDeviceContextRHI* InDeviceContextRHI, Vector2D InTextureSize )
	{
		SetPixelShaderValue( InDeviceContextRHI, texOffsetParameter, 1.f / InTextureSize );
	}

private:
	CShaderResourceParameter			bufferParameter;					/**< Buffer texture parameter */
	CShaderResourceParameter			bufferSamplerStateParameter;		/**< Buffer sampler sate parameter */
	CShaderParameter					texOffsetParameter;					/**< Texture offset parameter */
};

// #define avoids a lot of code duplication
#define VARIATION_PIXEL_SHADER_TYPE( HorizontalBlur ) \
	typedef TGaussianBlurPixelShader<HorizontalBlur>	TGaussianBlurPixelShader_##HorizontalBlur; \
	IMPLEMENT_SHADER_TYPE( template<>, TGaussianBlurPixelShader_##HorizontalBlur, TEXT( "GaussianBlurPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true )

VARIATION_PIXEL_SHADER_TYPE( false );
VARIATION_PIXEL_SHADER_TYPE( true );
#undef VARIATION_PIXEL_SHADER_TYPE

#endif // !GAUSSIANBLURSHADER_H

