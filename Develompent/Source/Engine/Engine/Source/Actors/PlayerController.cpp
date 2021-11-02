#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "System/BaseWindow.h"
#include "Actors/PlayerController.h"
#include "Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )

LCameraComponent* cameraComponent;

APlayerController::APlayerController()
{
	cameraComponent = CreateComponent< LCameraComponent >( TEXT( "CameraComponent0" ) );
	::cameraComponent = this->cameraComponent;
}
APlayerController::~APlayerController()
{}

void APlayerController::BeginPlay()
{
	Super::BeginPlay();

	GWindow->HideCursor();
	cameraComponent->SetAspectRatio( ( float )1280 / 720 );
	cameraComponent->SetProjectionMode( CPM_Perspective );
}

void APlayerController::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	// BS yehor.pohuliaka - This code for test and need refactor
	cameraComponent->RotateComponentByMouse();
	if ( GInputSystem->IsKeyDown( BC_KeyQ ) )
	{
		GIsRequestingExit = true;
	}
	if ( GInputSystem->IsKeyDown( BC_KeyS ) )
	{
		AddActorLocation( GetActorForwardVector() * -0.1f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyW ) )
	{
		AddActorLocation( GetActorForwardVector() * 0.1f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyA ) )
	{
		AddActorLocation( GetActorRightVector() * 0.1f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyD ) )
	{
		AddActorLocation( GetActorRightVector() * -0.1f );
	}
}