#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
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

void LCameraComponent::RotateComponentByMouse( bool InConstrainYaw /* = true */ )
{	
	const float		degress90InRadians		= 1.5708f;		// 1.5708 radians = 90 degrees
	const float		degress360InRadians		= 6.28319f;		// 6.28319 radians = 360 degrees

	FVector2D		mouseOffset = GInputSystem->GetMouseOffset();
	if ( mouseOffset.x == 0.f && mouseOffset.y == 0.f )
	{
		return;
	}

	float			sensitivity = GInputSystem->GetMouseSensitivity();
	FVector2D		eulerRotation = FMath::QuaternionToAngles( GetComponentRotation() );
	if ( mouseOffset.x != 0.f )
	{
		eulerRotation.y += FMath::DegreesToRadians( mouseOffset.x * sensitivity );
		if ( eulerRotation.x < -degress360InRadians || eulerRotation.x > degress360InRadians )
		{
			eulerRotation.x = 0.f;
		}
	}

	if ( mouseOffset.y != 0.f )
	{
		eulerRotation.x += FMath::DegreesToRadians( mouseOffset.y * sensitivity );
		if ( InConstrainYaw )
		{
			if ( eulerRotation.x > degress90InRadians )
			{
				eulerRotation.x = degress90InRadians;
			}
			else if ( eulerRotation.x < -degress90InRadians )
			{
				eulerRotation.x = -degress90InRadians;
			}
		}
		else if ( eulerRotation.x < -degress360InRadians || eulerRotation.x > degress360InRadians )
		{
			eulerRotation.x = 0.f;
		}
	}

	SetRotationComponent( FMath::AnglesToQuaternion( FVector( eulerRotation.x, eulerRotation.y, 0.f ) ) );
}

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