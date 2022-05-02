/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREPREVIEWVIEWPORTCLIENT_H
#define TEXTUREPREVIEWVIEWPORTCLIENT_H

#include "Math/Rotator.h"
#include "Render/Viewport.h"
#include "Render/Scene.h"
#include "Render/EditorCommonDrawHelper.h"
#include "System/WindowEvent.h"
#include "Render/Texture.h"

/**
 * @ingroup WorldEd
 * @brief Preview texture viewport client
 */
class FTexturePreviewViewportClient : public FViewportClient
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

protected:
	/**
	 * @brief Calculate scene view
	 *
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @return Return scene view
	 */
	class FSceneView* CalcSceneView( FViewport* InViewport );

	/**
	 * @brief Get background color
	 * @return Return background color
	 */
	FColor GetBackgroundColor() const;

	EShowFlags			showFlags;		/**< Show flags */
	FTexture2DRef		texture2D;		/**< Texture 2D to preview */
};

#endif // !TEXTUREPREVIEWVIEWPORTCLIENT_H