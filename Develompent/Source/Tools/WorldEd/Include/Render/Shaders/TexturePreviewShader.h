/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef TEXTUREPREVIEWSHADER_H
#define TEXTUREPREVIEWSHADER_H

#include "Core/Math/Color.h"
#include "Engine/Render/Shaders/ScreenShader.h"
#include "Engine/Render/Shaders/ShaderManager.h"

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

	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );

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

