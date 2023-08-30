/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREPREVIEWSHADER_H
#define TEXTUREPREVIEWSHADER_H

#include "Math/Color.h"
#include "Render/Shaders/ScreenShader.h"
#include "Render/Shaders/ShaderManager.h"

/**
 * @ingroup WorldEd
 * @brief Class of pixel shader for render preview texture
 */
class CTexturePreviewPixelShader : public CScreenPixelShader
{
	DECLARE_SHADER_TYPE( CTexturePreviewPixelShader )

public:
	/**
	 * @brief Initialize shader
	 * @param InShaderCacheItem			Cache of shader
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
	 * @brief Set color channel mask
	 * 
	 * @param InDeviceContextRHI	RHI device context
	 * @param InColorMask			Color mask
	 */
	FORCEINLINE void SetColorChannelMask( class CBaseDeviceContextRHI* InDeviceContextRHI, const CColor& InColorMask )
	{
		SetPixelShaderValue( InDeviceContextRHI, colorChannelMask, InColorMask.ToNormalizedVector4D() );
	}

	/**
	 * @brief Set mipmap to view
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InMipmap				Mipmap to view
	 */
	FORCEINLINE void SetMipmap( class CBaseDeviceContextRHI* InDeviceContextRHI, uint32 InMipmap )
	{
		SetPixelShaderValue( InDeviceContextRHI, mipmapToView, InMipmap );
	}

private:
	CShaderParameter		colorChannelMask;		/**< Color channel mask */
	CShaderParameter		mipmapToView;			/**< Mipmap to view */
};

#endif // !TEXTUREPREVIEWSHADER_H

