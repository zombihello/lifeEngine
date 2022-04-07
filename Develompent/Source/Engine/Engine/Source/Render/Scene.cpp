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
		float		halfWidth = InCameraView.orthoWidth / 2.f;
		float		halfHeight = InCameraView.orthoHeight / 2.f;
        projectionMatrix = glm::ortho( -halfWidth, halfWidth, -halfHeight, halfHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
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

	// Update frustum
	frustum.Update( viewProjectionMatrix );
}


void FScene::AddPrimitive( class LPrimitiveComponent* InPrimitive )
{
	check( InPrimitive );

	InPrimitive->LinkDrawList( this );
	primitives.push_back( InPrimitive );
}

void FScene::RemovePrimitive( class LPrimitiveComponent* InPrimitive )
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		if ( *it == InPrimitive )
		{
			InPrimitive->UnlinkDrawList();
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
		if ( InSceneView.GetFrustum().IsIn( ( *it )->GetBoundBox() ) )
		{
			( *it )->AddToDrawList( InSceneView );
		}
	}
}
