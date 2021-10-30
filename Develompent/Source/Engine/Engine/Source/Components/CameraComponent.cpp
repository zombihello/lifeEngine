#include "Components/CameraComponent.h"
#include "EngineDefines.h"

IMPLEMENT_CLASS( LCameraComponent )

LCameraComponent::LCameraComponent() :
	projectionMode( CPM_Perspective ),
	fieldOfView( 90.f ),
	orthoWidth( 512.f ),
	orthoHeight( 512.f ),
	nearClipPlane( 0.f ),
	farClipPlane( WORLD_MAX ),
	aspectRatio( 1.777778f )
{}

void LCameraComponent::GetCameraView( FCameraView& OutDesiredView )
{
	OutDesiredView.location			= GetComponentLocation();
	OutDesiredView.rotation			= GetComponentRotation();
	OutDesiredView.aspectRatio		= aspectRatio;
	OutDesiredView.farClipPlane		= farClipPlane;
	OutDesiredView.fov				= fieldOfView;
	OutDesiredView.nearClipPlane	= nearClipPlane;
	OutDesiredView.orthoHeight		= orthoHeight;
	OutDesiredView.orthoWidth		= orthoWidth;
	OutDesiredView.projectionMode	= projectionMode;
}