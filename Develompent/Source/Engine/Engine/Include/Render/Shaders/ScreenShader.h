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
 * @brief Enumeration of screen vertex shader type
 */
enum EScreenVertexShaderType
{
	SVST_Default,			/**< Default shader for render primitives */
	SVST_Fullscreen			/**< Vertex shader for render one triangle on full viewport size without geometry on input */
};

/**
 * @ingroup Engine
 * @brief Class of vertex shader for render in screen space
 */
template< EScreenVertexShaderType InType >
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
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr )
	{
		if ( !InVFMetaType )
		{
			return true;
		}

		return InVFMetaType->GetHash() == FSimpleElementVertexFactory::staticType.GetHash();
	}
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

IMPLEMENT_SHADER_TYPE( template<>, FScreenVertexShader<SVST_Default>, TEXT( "ScreenVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( template<>, FScreenVertexShader<SVST_Fullscreen>, TEXT( "ScreenVertexShader.hlsl" ), TEXT( "FullscreenMainVS" ), SF_Vertex, true );

#endif // !SCREENSHADER_H

