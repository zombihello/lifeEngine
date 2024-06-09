/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BLOOMSHADER_H
#define BLOOMSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Bloom pixel shader
 */
class CBloomPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CBloomPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param InShaderCacheItem		Cache of shader
	 */
	virtual void Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem ) override;

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );
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
	 * @brief Set bright threshold
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InBrightThreshold		New bright threshold
	 */
	FORCEINLINE void SetBrightThreshold( class CBaseDeviceContextRHI* InDeviceContextRHI, Vector InBrightThreshold )
	{
		SetPixelShaderValue( InDeviceContextRHI, brightThresholdParameter, InBrightThreshold );
	}

private:
	CShaderResourceParameter			sceneColorParameter;					/**< Scene color texture parameter */
	CShaderResourceParameter			sceneColorSamplerStateParameter;		/**< Scene color sampler sate parameter */
	CShaderParameter					brightThresholdParameter;				/**< Bright threshold parameter */
};

#endif // !BLOOMSHADER_H

