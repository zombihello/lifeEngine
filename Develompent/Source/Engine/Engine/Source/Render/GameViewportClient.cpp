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

void FGameViewportClient::Draw( FViewport* InViewport )
{
	// Create scene view for draw scene
	FCameraView			cameraView;
	LCameraComponent*	cameraComponent = GCameraManager->GetActiveCamera();
	if ( cameraComponent )
	{
		cameraComponent->GetCameraView( cameraView );
	}

	// Calculate scene view and update audio listener spatial
	FSceneView*		sceneView = CalcSceneView( InViewport, cameraView );
	GAudioDevice.SetListenerSpatial( cameraView.location, cameraView.rotation.RotateVector( FMath::vectorForward ), cameraView.rotation.RotateVector( FMath::vectorUp ) );

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FViewportRenderCommand,
										  FGameViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void FGameViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView, ( FScene* )GWorld->GetScene() );

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

FSceneView* FGameViewportClient::CalcSceneView( FViewport* InViewport, const FCameraView& InCameraView )
{
	// Calculate projection matrix
	FMatrix		projectionMatrix;
	if ( InCameraView.projectionMode == CPM_Perspective )
	{
		projectionMatrix		= glm::perspective( FMath::DegreesToRadians( InCameraView.fov ), InCameraView.aspectRatio, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}
	else
	{
		float		halfWidth	= InCameraView.orthoWidth / 2.f;
		float		halfHeight	= InCameraView.orthoHeight / 2.f;
		projectionMatrix		= glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}

	// Update view matrix
	FVector		targetDirection		= InCameraView.rotation.RotateVector( FMath::vectorForward );
	FVector		axisUp				= InCameraView.rotation.RotateVector( FMath::vectorUp );
	FMatrix		viewMatrix			= glm::lookAt( InCameraView.location, InCameraView.location + targetDirection, axisUp );

	FSceneView*		sceneView = new FSceneView( projectionMatrix, viewMatrix, InViewport->GetSizeX(), InViewport->GetSizeY(), FColor::black, SHOW_DefaultGame );
	return sceneView;
}