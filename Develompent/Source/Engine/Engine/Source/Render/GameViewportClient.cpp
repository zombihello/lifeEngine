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

void CGameViewportClient::Draw( CViewport* InViewport )
{
	// Create scene view for draw scene
	SCameraView			cameraView;
	CCameraComponent*	cameraComponent = GCameraManager->GetActiveCamera();
	if ( cameraComponent )
	{
		cameraComponent->GetCameraView( cameraView );
	}

	// Calculate scene view and update audio listener spatial
	CSceneView*		sceneView = CalcSceneView( InViewport, cameraView );
	GAudioDevice.SetListenerSpatial( cameraView.location, cameraView.rotation.RotateVector( SMath::vectorForward ), cameraView.rotation.RotateVector( SMath::vectorUp ) );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( CViewportRenderCommand,
										  CGameViewportClient*, viewportClient, this,
										  ViewportRHIRef_t, viewportRHI, InViewport->GetViewportRHI(),
										  CSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void CGameViewportClient::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, CSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	CBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	CSceneRenderer				sceneRenderer( InSceneView, ( CScene* )GWorld->GetScene() );

	// Scene render
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	sceneRenderer.Render( InViewportRHI );

	// UI render
	GUIEngine->BeginDraw();
	GUIEngine->EndDraw();
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );

	// Delete scene view
	delete InSceneView;
}

CSceneView* CGameViewportClient::CalcSceneView( CViewport* InViewport, const SCameraView& InCameraView )
{
	// Calculate projection matrix
	Matrix		projectionMatrix;
	if ( InCameraView.projectionMode == CPM_Perspective )
	{
		projectionMatrix		= glm::perspective( SMath::DegreesToRadians( InCameraView.fov ), InCameraView.aspectRatio, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}
	else
	{
		float		halfWidth	= InCameraView.orthoWidth / 2.f;
		float		halfHeight	= InCameraView.orthoHeight / 2.f;
		projectionMatrix		= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}

	// Update view matrix
	Vector		targetDirection		= InCameraView.rotation.RotateVector( SMath::vectorForward );
	Vector		axisUp				= InCameraView.rotation.RotateVector( SMath::vectorUp );
	Matrix		viewMatrix			= glm::lookAt( InCameraView.location, InCameraView.location + targetDirection, axisUp );

	CSceneView*		sceneView = new CSceneView( projectionMatrix, viewMatrix, InViewport->GetSizeX(), InViewport->GetSizeY(), ÑColor::black, SHOW_DefaultGame );
	return sceneView;
}