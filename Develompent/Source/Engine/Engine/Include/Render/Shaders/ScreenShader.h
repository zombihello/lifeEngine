/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCREENSHADER_H
#define SCREENSHADER_H

#include "Shader.h"
#include "ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Class of vertex shader for render in screen space
 */
class FScreenVertexShader : public FShader
{
	DECLARE_SHADER_TYPE( FScreenVertexShader )
};

/**
 * @ingroup Engine
 * @brief Class of pixel shader for render in screen space
 */
class FScreenPixelShader : public FShader
{
	DECLARE_SHADER_TYPE( FScreenPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem ) override;

	/**
	 * @brief Set texture parameter
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InTextureRHI			RHI Texture
	 */
	FORCEINLINE void SetTexture( class FBaseDeviceContextRHI* InDeviceContextRHI, const FTexture2DRHIParamRef InTextureRHI )
	{
		SetTextureParameter( InDeviceContextRHI, textureParameter, InTextureRHI );
	}

	/**
	 * @brief Set sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetSamplerState( class FBaseDeviceContextRHI* InDeviceContextRHI, const FSamplerStateRHIParamRef InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, samplerStateParameter, InSamplerStateRHI );
	}

private:
	FShaderResourceParameter			textureParameter;			/**< Texture parameter */
	FShaderResourceParameter			samplerStateParameter;		/**< Sampler sate parameter */
};

#endif // !SCREENSHADER_H

