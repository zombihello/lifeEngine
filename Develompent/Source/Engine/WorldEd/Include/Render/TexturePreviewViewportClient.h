/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREPREVIEWVIEWPORTCLIENT_H
#define TEXTUREPREVIEWVIEWPORTCLIENT_H

#include "Render/EditorLevelViewportClient.h"
#include "Render/Texture.h"

/**
 * @ingroup WorldEd
 * @brief Preview texture viewport client
 */
class FTexturePreviewViewportClient : public FEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InTexture2D		Texture 2D to preview
	 */
	FTexturePreviewViewportClient( FTexture2D* InTexture2D );

	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( FViewport* InViewport ) override;

	/**
	 * @brief Draw viewport. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InTexture2D		Texture 2D to preview
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( FViewportRHIRef InViewportRHI, FTexture2D* InTexture2D, class FSceneView* InSceneView );

	/**
	 * @brief Set texture 2D to preview
	 * @param InTexture2D		Texture 2D to preview
	 */
	FORCEINLINE void SetTexture2D( FTexture2D* InTexture2D )
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

protected:
	/**
	 * @brief Calculate scene view
	 *
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @return Return scene view
	 */
	virtual class FSceneView* CalcSceneView( FViewport* InViewport ) override;

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	virtual FColor GetBackgroundColor() const override;

	FTexture2DRef		texture2D;			/**< Texture 2D to preview */
	FColor				colorChannelMask;	/**< Color channel mask */
};

#endif // !TEXTUREPREVIEWVIEWPORTCLIENT_H