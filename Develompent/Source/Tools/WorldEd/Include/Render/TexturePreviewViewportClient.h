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

#ifndef TEXTUREPREVIEWVIEWPORTCLIENT_H
#define TEXTUREPREVIEWVIEWPORTCLIENT_H

#include "Engine/Render/Texture.h"
#include "Render/EditorLevelViewportClient.h"

/**
 * @ingroup WorldEd
 * @brief Preview texture viewport client
 */
class CTexturePreviewViewportClient : public CEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InTexture2D		Texture 2D to preview
	 */
	CTexturePreviewViewportClient( const TSharedPtr<CTexture2D>& InTexture2D );

	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;

	/**
	 * @brief Draw viewport. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InTexture2D		Texture 2D to preview
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( ViewportRHIRef_t InViewportRHI, const TSharedPtr<CTexture2D>& InTexture2D, class CSceneView* InSceneView );

	/**
	 * @brief Set texture 2D to preview
	 * @param InTexture2D		Texture 2D to preview
	 */
	FORCEINLINE void SetTexture2D( const TSharedPtr<CTexture2D>& InTexture2D )
	{
		texture2D = InTexture2D;
	}

	/**
	 * @brief Show red channel
	 * @param InShowRedChannel		Is need show red channel
	 */
	FORCEINLINE void ShowRedChannel( bool InShowRedChannel )
	{
		colorChannelMask.r = InShowRedChannel ? 255 : 0;
	}

	/**
	 * @brief Show green channel
	 * @param InShowGreenChannel		Is need show green channel
	 */
	FORCEINLINE void ShowGreenChannel( bool InShowGreenChannel )
	{
		colorChannelMask.g = InShowGreenChannel ? 255 : 0;
	}

	/**
	 * @brief Show blue channel
	 * @param InShowBlueChannel		Is need show blue channel
	 */
	FORCEINLINE void ShowBlueChannel( bool InShowBlueChannel )
	{
		colorChannelMask.b = InShowBlueChannel ? 255 : 0;
	}

	/**
	 * @brief Show alpha channel
	 * @param InShowAlphaChannel		Is need show alpha channel
	 */
	FORCEINLINE void ShowAlphaChannel( bool InShowAlphaChannel )
	{
		colorChannelMask.a = InShowAlphaChannel ? 255 : 0;
	}

	/**
	 * @brief Set mipmap
	 * @param InMipmap	Mipmap to view
	 */
	FORCEINLINE void SetMipmap( uint32 InMipmap )
	{
		mipmapToView = InMipmap;
	}

	/**
	 * @brief Is showed red channel
	 * @return Return TRUE if showed red channel, else return FALSE
	 */
	FORCEINLINE bool IsShowRedChannel() const
	{
		return colorChannelMask.r > 0;
	}

	/**
	 * @brief Is showed green channel
	 * @return Return TRUE if showed green channel, else return FALSE
	 */
	FORCEINLINE bool IsShowGreenChannel() const
	{
		return colorChannelMask.g > 0;
	}

	/**
	 * @brief Is showed blue channel
	 * @return Return TRUE if showed blue channel, else return FALSE
	 */
	FORCEINLINE bool IsShowBlueChannel() const
	{
		return colorChannelMask.b > 0;
	}

	/**
	 * @brief Is showed alpha channel
	 * @return Return TRUE if showed alpha channel, else return FALSE
	 */
	FORCEINLINE bool IsShowAlphaChannel() const
	{
		return colorChannelMask.a > 0;
	}

	/**
	 * @brief Get mipmap
	 * @return Return mipmap for view
	 */
	FORCEINLINE uint32 GetMipmap() const
	{
		return mipmapToView;
	}

protected:
	/**
	 * @brief Calculate scene view
	 *
	 * @warning Need delete allocated pointer
	 * @param InSizeX	Size X
	 * @param InSizeY	Size Y
	 * @return Return scene view
	 */
	virtual class CSceneView* CalcSceneView( uint32 InSizeX, uint32 InSizeY ) override;

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	virtual CColor GetBackgroundColor() const override;

	TSharedPtr<CTexture2D>		texture2D;			/**< Texture 2D to preview */
	CColor						colorChannelMask;	/**< Color channel mask */
	uint32						mipmapToView;		/**< Mipmap to view */
};

#endif // !TEXTUREPREVIEWVIEWPORTCLIENT_H