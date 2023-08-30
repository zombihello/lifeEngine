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
class CGameViewportClient : public CViewportClient
{
public:
	/**
	 * Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;

	/**
	 * Draw viewport. Must be call in render thread
	 * 
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 */
	void Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView );

private:
	/**
	 * @brief Calculate scene view
	 *
	 * @warning Need delete allocated pointer
	 * @param InViewport		Viewport
	 * @param InCameraView		Camera view
	 * @return Return scene view
	 */
	class CSceneView* CalcSceneView( CViewport* InViewport, const struct CameraView& InCameraView );
};

#endif // !GAMEVIEWPORTCLIENT_H