#include "Misc/AudioGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Render/EditorLevelViewportClient.h"
#include "Render/Scene.h"
#include "Render/SceneRendering.h"
#include "Render/RenderingThread.h"
#include "Render/WorldGrid.h"
#include "Render/WorldGridDrawingPolicy.h"
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

	// Scene render
	sceneRenderer.BeginRenderViewTarget( InViewportRHI );
	sceneRenderer.Render( InViewportRHI );

	// Render world grid
	{
		SCOPED_DRAW_EVENT( EventWorldGrid, DEC_SCENE_ITEMS, TEXT( "World grid" ) );
		FWorldGrid*													worldGrid = GEditorEngine->GetWorldGrid();
		FMeshBatch													worldGridMeshBatch = worldGrid->GetMeshBatch();
		FColor														colorGrid( 130, 130, 130 );
		worldGridMeshBatch.numInstances = 1;

		switch ( viewportType )
		{
		case LVT_Perspective:
			colorGrid = FColor( 0, 0, 100 );

		case LVT_OrthoXZ:
			worldGridMeshBatch.transformationMatrices.push_back( FMath::matrixIdentity );
			break;

		case  LVT_OrthoXY:
			worldGridMeshBatch.transformationMatrices.push_back( FRotator( 90.f, 0.f, 0.f ).ToMatrix() );
			break;

		case LVT_OrthoYZ:
			worldGridMeshBatch.transformationMatrices.push_back( FRotator( 90.f, 0.f, 90.f ).ToMatrix() );
			break;

		default:
			check( false );			// Unknown viewport type
			break;
		}
		

		TWireframeMeshDrawingPolicy< FWorldGridDrawingPolicy >		worldGridDrawingPolicy;
		worldGridDrawingPolicy.Init( worldGrid->GetVertexFactory(), colorGrid );
		worldGridDrawingPolicy.SetRenderState( immediateContext );
		worldGridDrawingPolicy.SetShaderParameters( immediateContext );
		worldGridDrawingPolicy.Draw( immediateContext, worldGridMeshBatch, *InSceneView );
	}
	sceneRenderer.FinishRenderViewTarget( InViewportRHI );

	// Delete scene view
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
		float		halfWidth	= viewportSizeX / 2.f;
		float		halfHeight	= viewportSizeY / 2.f;
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

	FSceneView*		sceneView = new FSceneView( projectionMatrix, viewMatrix, GetBackgroundColor(), showFlags );
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