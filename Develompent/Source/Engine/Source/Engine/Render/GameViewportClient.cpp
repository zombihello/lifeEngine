/**
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

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/GameViewportClient.h"
#include "Engine/Render/Scene.h"
#include "Engine/Render/SceneRendering.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/System/CameraManager.h"
#include "Engine/System/World.h"
#include "Audio/Misc/AudioGlobals.h"
#include "Audio/System/AudioDevice.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/UIEngine.h"

/*
==================
CGameViewportClient::Draw
==================
*/
void CGameViewportClient::Draw( CViewport* InViewport )
{
	// Create scene view for draw scene
	CameraView			cameraView;
	CCameraComponent*	cameraComponent = g_CameraManager->GetActiveCamera();
	if ( cameraComponent )
	{
		cameraComponent->GetCameraView( cameraView );
	}

	// Calculate scene view and update audio listener spatial
	CSceneView*		sceneView = CalcSceneView( InViewport, cameraView );
	g_AudioDevice.SetListenerSpatial( cameraView.location, cameraView.rotation * Math::vectorForward, cameraView.rotation * Math::vectorUp );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CGameViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

/*
==================
CGameViewportClient::Draw_RenderThread
==================
*/
void CGameViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, CSceneView* InSceneView )
{
	Assert( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = g_RHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, ( CScene* )g_World->GetScene() );

	// Scene render
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	sceneRenderer.Render( InViewportRHI );

	// UI render
	g_UIEngine->BeginDraw();
	g_UIEngine->EndDraw();
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );

	// Delete scene view
	delete InSceneView;
}

/*
==================
CGameViewportClient::CalcSceneView
==================
*/
CSceneView* CGameViewportClient::CalcSceneView( CViewport* InViewport, const CameraView& InCameraView )
{
	// Calculate projection matrix
	Matrix		projectionMatrix;
	if ( InCameraView.projectionMode == CPM_Perspective )
	{
		projectionMatrix		= glm::perspective( Math::DegreesToRadians( InCameraView.fov ), InCameraView.aspectRatio, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}
	else
	{
		float		halfWidth	= InCameraView.orthoWidth / 2.f;
		float		halfHeight	= InCameraView.orthoHeight / 2.f;
		projectionMatrix		= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}

	// Update view matrix
	Vector		targetDirection		= InCameraView.rotation * Math::vectorForward;
	Vector		axisUp				= InCameraView.rotation * Math::vectorUp;
	Matrix		viewMatrix			= glm::lookAt( InCameraView.location, InCameraView.location + targetDirection, axisUp );

	CSceneView*		sceneView = new CSceneView( InCameraView.location, projectionMatrix, viewMatrix, InViewport->GetSizeX(), InViewport->GetSizeY(), CColor::black, SHOW_DefaultGame );
	return sceneView;
}