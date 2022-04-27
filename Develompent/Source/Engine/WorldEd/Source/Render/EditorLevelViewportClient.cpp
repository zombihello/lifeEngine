#include "Misc/AudioGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Render/EditorLevelViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "Render/SceneUtils.h"
#include "System/AudioDevice.h"
#include "System/EditorEngine.h"
#include "EngineDefines.h"

FEditorLevelViewportClient::FEditorLevelViewportClient()
	: bSetListenerPosition( true )
	, viewportType( LVT_Perspective )
	, viewLocation( FMath::vectorZero )
	, viewRotation( FMath::rotatorZero )
	, viewFOV( 90.f )
	, orthoZoom( 10000.f )
	, showFlags( SHOW_DefaultEditor )
{}

void FEditorLevelViewportClient::Draw( FViewport* InViewport )
{
	FSceneView*		sceneView = CalcSceneView( InViewport );

	// Update audio listener spatial if allowed
	if ( viewportType == LVT_Perspective && bSetListenerPosition )
	{
		GAudioDevice.SetListenerSpatial( viewLocation, viewRotation.RotateVector( FMath::vectorForward ), viewRotation.RotateVector( FMath::vectorUp ) );
	}

	// Draw viewport
	UNIQUE_RENDER_COMMAND_THREEPARAMETER( FViewportRenderCommand,
										  FEditorLevelViewportClient*, viewportClient, this,
										  FViewportRHIRef, viewportRHI, InViewport->GetViewportRHI(),
										  FSceneView*, sceneView, sceneView,
										  {
											  viewportClient->Draw_RenderThread( viewportRHI, sceneView );
										  } );
}

void FEditorLevelViewportClient::Draw_RenderThread( FViewportRHIRef InViewportRHI, class FSceneView* InSceneView )
{
	check( IsInRenderingThread() );
	FBaseDeviceContextRHI*		immediateContext = GRHI->GetImmediateContext();
	FSceneRenderer				sceneRenderer( InSceneView );
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	
	// Draw grid
	drawHelper.DrawGrid( InSceneView, viewportType );
	
	// Draw scene
	sceneRenderer.Render( InViewportRHI );
	
	// Finishing render and delete scene view
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );
	delete InSceneView;
}

FSceneView* FEditorLevelViewportClient::CalcSceneView( FViewport* InViewport )
{
	// Calculate projection matrix
	FMatrix		projectionMatrix;
	float		viewportSizeX	= InViewport->GetSizeX();
	float		viewportSizeY	= InViewport->GetSizeY();
	if ( viewportType == LVT_Perspective )
	{
		projectionMatrix = glm::perspective( FMath::DegreesToRadians( viewFOV ), viewportSizeX / viewportSizeY, 0.01f, ( float )WORLD_MAX );
	}
	else
	{
		const float		zoom		= orthoZoom / ( viewportSizeX * 15.0f );
		float			halfWidth	= zoom * viewportSizeX / 2.f;
		float			halfHeight	= zoom * viewportSizeY / 2.f;
		projectionMatrix = glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, ( float )-HALF_WORLD_MAX, ( float )HALF_WORLD_MAX );
	}

	// Calculate view matrix
	FMatrix		viewMatrix;
	FVector		targetDirection;
	FVector		axisUp;
	switch ( viewportType )
	{
	case LVT_Perspective:
	case LVT_OrthoXY:
		targetDirection		= viewRotation.RotateVector( FMath::vectorForward );
		axisUp				= viewRotation.RotateVector( FMath::vectorUp );
		break;

	case LVT_OrthoXZ:
		targetDirection		= viewRotation.RotateVector( -FMath::vectorUp );
		axisUp				= viewRotation.RotateVector( FMath::vectorForward );
		break;

	case LVT_OrthoYZ:
		targetDirection		= viewRotation.RotateVector( FMath::vectorRight );
		axisUp				= viewRotation.RotateVector( FMath::vectorUp );
		break;

	default:
		check( false );		// Unknown viewport type
		break;
	}

	viewMatrix = glm::lookAt( viewLocation, viewLocation + targetDirection, axisUp );

	FSceneView*		sceneView = new FSceneView( projectionMatrix, viewMatrix, InViewport->GetSizeX(), InViewport->GetSizeY(), GetBackgroundColor(), showFlags );
	return sceneView;
}

FColor FEditorLevelViewportClient::GetBackgroundColor() const
{
	if ( viewportType == LVT_Perspective )
	{
		return FColor::black;
	}
	else
	{
		return FColor( 163, 163, 163 );
	}
}