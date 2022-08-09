#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "Components/CameraComponent.h"
#include "EngineDefines.h"

IMPLEMENT_CLASS( CCameraComponent )

CCameraComponent::CCameraComponent()
    : bIsActive( false )
	, bAutoViewData( false )
    , projectionMode( CPM_Perspective )
    , fieldOfView( 90.f )
    , orthoWidth( 512.f )
    , orthoHeight( 512.f )
    , nearClipPlane( 0.01f )
    , farClipPlane( WORLD_MAX )
    , aspectRatio( 1.777778f )
	, rotateEuler( SMath::vectorZero ) 
{}

void CCameraComponent::RotateComponentByMouse( bool InConstrainYaw /* = true */ )
{	
	Vector2D		mouseOffset = GInputSystem->GetMouseOffset();
	if ( mouseOffset.x == 0.f && mouseOffset.y == 0.f )
	{
		return;
	}

	float			sensitivity = GInputSystem->GetMouseSensitivity();
	
	// Update Yaw axis
	if ( mouseOffset.x != 0.f )
	{
		rotateEuler.y += mouseOffset.x * sensitivity;
		if ( rotateEuler.y < -360.f || rotateEuler.y > 360.f )
		{
			rotateEuler.y = 0.f;
		}
	}

	// Update Pitch axis
	if ( mouseOffset.y != 0.f )
	{
		rotateEuler.x -= mouseOffset.y * sensitivity;
		if ( InConstrainYaw )
		{
			if ( rotateEuler.x > 90.f )
			{
				rotateEuler.x = 90.f;
			}
			else if ( rotateEuler.x < -90.f )
			{
				rotateEuler.x = -90.f;
			}
		}
		else if ( rotateEuler.x < -360.f || rotateEuler.x > 360.f )
		{
			rotateEuler.x = 0.f;
		}

	}

	SetRelativeRotation( SMath::AnglesToQuaternionZXY( rotateEuler ) );
}

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
