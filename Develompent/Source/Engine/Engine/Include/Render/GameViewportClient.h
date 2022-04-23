/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GAMEVIEWPORTCLIENT_H
#define GAMEVIEWPORTCLIENT_H

#include "Render/Viewport.h"

/**
 * @ingroup Engine
 * Game viewport client
 */
class FGameViewportClient : public FViewportClient
{
public:
	/**
	 * Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( FViewport* InViewport ) override;

	/**
	 * Draw viewport. Must be call in render thread
	 * 
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView );

private:
	/**
	 * @brief Calculate scene view
	 *
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @param InCameraView		Camera view
	 * @return Return scene view
	 */
	class FSceneView* CalcSceneView( FViewport* InViewport, const struct FCameraView& InCameraView );
};

#endif // !GAMEVIEWPORTCLIENT_H