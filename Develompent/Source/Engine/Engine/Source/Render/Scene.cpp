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
	// Update projection matrix
	if ( InCameraView.projectionMode == CPM_Perspective )
	{
		projectionMatrix = glm::perspective( glm::radians( InCameraView.fov ), InCameraView.aspectRatio, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}
	else
	{
		projectionMatrix = glm::ortho( 0.f, InCameraView.orthoWidth, 0.f, InCameraView.orthoHeight, InCameraView.nearClipPlane, InCameraView.farClipPlane );
	}

	// Update view matrix
	{
		// BS yehor.pohuliaka - Maybe it not correct work
		glm::vec3		targetDirection = glm::vec3( 0.f, 0.f, -1.f ) * InCameraView.rotation;
		glm::vec3		axisUp = glm::vec3( 0.f, 1.f, 0.f ) * InCameraView.rotation;
		glm::vec3		axisRight = glm::normalize( glm::cross( targetDirection, glm::vec3( 0.f, 1.f, 0.f ) ) );
		viewMatrix = glm::lookAt( InCameraView.location, InCameraView.location + targetDirection, axisUp );
	}

	// Update view*projection matrix
	viewProjectionMatrix = projectionMatrix * viewMatrix;

	// Copy view info
	cameraView = InCameraView;
}