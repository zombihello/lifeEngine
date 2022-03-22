#include "Render/Scene.h"

FSceneView::FSceneView() :
	viewMatrix( FMath::matrixIdentity ),
	projectionMatrix( FMath::matrixIdentity ),
	viewProjectionMatrix( FMath::matrixIdentity )
{
	appMemzero( &cameraView, sizeof( FCameraView ) );
}

void FSceneView::SetCameraView( const FCameraView& InCameraView )
{
    // Update projection and view matrix
    if ( InCameraView.projectionMode == CPM_Perspective )
    {
        projectionMatrix = glm::perspective( FMath::DegreesToRadians( InCameraView.fov ), InCameraView.aspectRatio, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}
	else
	{
        projectionMatrix = glm::ortho( 0.f, InCameraView.orthoWidth, 0.f, InCameraView.orthoHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}

    // Update view matrix
    {
        FVector		targetDirection		= InCameraView.rotation.RotateVector( FMath::vectorForward );
        FVector		axisUp				= InCameraView.rotation.RotateVector( FMath::vectorUp );
        viewMatrix = glm::lookAt( InCameraView.location, InCameraView.location + targetDirection, axisUp );
    }

	// Update view*projection matrix
    viewProjectionMatrix = projectionMatrix * viewMatrix;

	// Copy view info
	cameraView = InCameraView;
}


void FScene::AddPrimitive( class LPrimitiveComponent* InPrimitive )
{
	primitives.push_back( InPrimitive );
}

void FScene::RemovePrimitive( class LPrimitiveComponent* InPrimitive )
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		if ( *it == InPrimitive )
		{
			primitives.erase( it );
			return;
		}
	}
}

void FScene::BuildSDGs( const FSceneView& InSceneView )
{
	// Clear scene depth groups
	for ( uint32 index = 0; index < SDG_Max; ++index )
	{
		SDGs[ index ].Clear();
	}

	// Add to SDGs primitives
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
        ( *it )->AddToDrawList( this, InSceneView );
	}
}
