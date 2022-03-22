#include "Actors/HEPlayerController.h"

IMPLEMENT_CLASS( AHEPlayerController )

void AHEPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	LCameraComponent*		cameraComponent = GetCameraComponent();
	cameraComponent->SetProjectionMode( CPM_Orthographic );
	cameraComponent->SetIgnoreRotateByMouse( true );
	cameraComponent->SetNearClipPlane( -100.f );
	cameraComponent->SetFarClipPlane( 100.f );
}