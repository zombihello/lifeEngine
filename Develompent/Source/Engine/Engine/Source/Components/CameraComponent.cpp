#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "Components/CameraComponent.h"
#include "EngineDefines.h"

IMPLEMENT_CLASS( CCameraComponent )
IMPLEMENT_ENUM( ECameraProjectionMode, FOREACH_ENUM_CAMERAPROJECTMODE )

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
CCameraComponent::StaticInitializeClass
==================
*/
void CCameraComponent::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "bIsActive" ) )			CBoolProperty( CPP_PROPERTY( ThisClass, bIsActive ), TEXT( "Camera" ), TEXT( "Is active camera. This field setted from CCameraManager::SetActiveCamera" ), CPF_Edit );
	new( staticClass, TEXT( "bAutoViewData" ) )		CBoolProperty( CPP_PROPERTY( ThisClass, bAutoViewData ), TEXT( "Camera" ), TEXT( "Auto view data. If this flag is true, CCameraManager auto set aspect ratio (for CPM_Perspective) or set ortho width/height (for CPM_Orthographic)" ), CPF_Edit );
	new( staticClass, TEXT( "Projection Mode" ) )	CByteProperty( CPP_PROPERTY( ThisClass, projectionMode ), TEXT( "Camera" ), TEXT( "Projection mode" ), CPF_Edit, Enum::ECameraProjectionMode::StaticEnum() );
	new( staticClass, TEXT( "Field Of View" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, fieldOfView ), TEXT( "Camera" ), TEXT( "The horizontal field of view (in degrees) in perspective mode (ignored in Orthographic mode)" ), CPF_Edit );
	new( staticClass, TEXT( "Ortho Width" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, orthoWidth ), TEXT( "Camera" ), TEXT( "The desired width (in world units) of the orthographic view (ignored in Perspective mode)" ), CPF_Edit );
	new( staticClass, TEXT( "Ortho Height" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, orthoHeight ), TEXT( "Camera" ), TEXT( "The desired height (in world units) of the orthographic view (ignored in Perspective mode)" ), CPF_Edit );
	new( staticClass, TEXT( "Near Clip Plane" ) )	CFloatProperty( CPP_PROPERTY( ThisClass, nearClipPlane ), TEXT( "Camera" ), TEXT( "The near plane distance (in world units)" ), CPF_Edit );
	new( staticClass, TEXT( "Far Clip Plane" ) )	CFloatProperty( CPP_PROPERTY( ThisClass, farClipPlane ), TEXT( "Camera" ), TEXT( "The far plane distance (in world units)" ), CPF_Edit );
	new( staticClass, TEXT( "Aspect Ratio" ) )		CFloatProperty( CPP_PROPERTY( ThisClass, aspectRatio ), TEXT( "Camera" ), TEXT( "Aspect Ratio (Width/Height) (ignored in Orthographic mode)" ), CPF_Edit );
}

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
void CCameraComponent::GetCameraView( CameraView& OutDesiredView )
{
	OutDesiredView.location			= GetComponentLocation();
	OutDesiredView.rotation			= GetComponentQuat();
	OutDesiredView.aspectRatio		= aspectRatio;
	OutDesiredView.farClipPlane		= farClipPlane;
	OutDesiredView.fov				= fieldOfView;
	OutDesiredView.nearClipPlane	= nearClipPlane;
	OutDesiredView.orthoHeight		= orthoHeight;
	OutDesiredView.orthoWidth		= orthoWidth;
	OutDesiredView.projectionMode	= projectionMode;
}
