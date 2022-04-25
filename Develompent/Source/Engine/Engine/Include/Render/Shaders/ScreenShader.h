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

public:
#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR
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

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr );
#endif // WITH_EDITOR

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

