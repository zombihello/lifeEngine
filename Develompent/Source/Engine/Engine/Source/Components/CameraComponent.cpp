#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "Components/CameraComponent.h"
#include "EngineDefines.h"

IMPLEMENT_CLASS( CCameraComponent )

/*
==================
CCameraComponent::CCameraComponent
==================
*/
CCameraComponent::CCameraComponent()
    : bIsActive( false )
	, bAutoViewData( false )
    , projectionMode( CPM_Perspective )
    , fieldOfView( 90.f )
    , orthoWidth( 512.f )
    , orthoHeight( 512.f )
    , nearClipPlane( 0.01f )
    , farClipPlane( HALF_WORLD_MAX )
    , aspectRatio( 1.777778f )
{}

/*
==================
CCameraComponent::Serialize
==================
*/
void CCameraComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );

	InArchive << bIsActive;
	InArchive << bAutoViewData;
	InArchive << projectionMode;
	InArchive << fieldOfView;
	InArchive << orthoWidth;
	InArchive << orthoHeight;
	InArchive << nearClipPlane;
	InArchive << farClipPlane;
	InArchive << aspectRatio;

	if ( nearClipPlane <= 0.f )
	{
		nearClipPlane = 0.01f;
	}
}

/*
==================
CCameraComponent::GetCameraView
==================
*/
void CCameraComponent::GetCameraView( SCameraView& OutDesiredView )
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
