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
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

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
class CScreenVertexShader : public CShader
{
	DECLARE_SHADER_TYPE( CScreenVertexShader )

public:
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
#endif // WITH_EDITOR
};

/**
 * @ingroup Engine
 * @brief Class of pixel shader for render in screen space
 */
class CScreenPixelShader : public CShader
{
	DECLARE_SHADER_TYPE( CScreenPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
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
		SetTextureParameter( InDeviceContextRHI, textureParameter, InTextureRHI );
	}

	/**
	 * @brief Set sampler state parameter
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InSamplerStateRHI		RHI sampler state
	 */
	FORCEINLINE void SetSamplerState( class CBaseDeviceContextRHI* InDeviceContextRHI, const SamplerStateRHIParamRef_t InSamplerStateRHI )
	{
		SetSamplerStateParameter( InDeviceContextRHI, samplerStateParameter, InSamplerStateRHI );
	}

private:
	CShaderResourceParameter			textureParameter;			/**< Texture parameter */
	CShaderResourceParameter			samplerStateParameter;		/**< Sampler sate parameter */
};

IMPLEMENT_SHADER_TYPE( template<>, CScreenVertexShader<SVST_Default>, TEXT( "ScreenVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( template<>, CScreenVertexShader<SVST_Fullscreen>, TEXT( "ScreenVertexShader.hlsl" ), TEXT( "FullscreenMainVS" ), SF_Vertex, true );

#endif // !SCREENSHADER_H

