#include "Misc/EngineGlobals.h"
#include "Misc/AudioGlobals.h"
#include "Misc/UIGlobals.h"
#include "Render/GameViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "System/AudioDevice.h"
#include "System/CameraManager.h"
#include "System/World.h"
#include "UIEngine.h"

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