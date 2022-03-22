#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "System/BaseWindow.h"
#include "Actors/PlayerController.h"
#include "Components/CameraComponent.h"
#include "System/CameraManager.h"
#include "Misc/EngineGlobals.h"

IMPLEMENT_CLASS( APlayerController )

APlayerController::APlayerController()
{
	cameraComponent = CreateComponent< LCameraComponent >( TEXT( "CameraComponent0" ) );   
}

APlayerController::~APlayerController()
{
	// Deactive player camera if need
	if ( cameraComponent->IsActive() )
	{
		GCameraManager->SetActiveCamera( nullptr );
	}
}

void APlayerController::BeginPlay()
{
	Super::BeginPlay();

	GWindow->HideCursor();
	GCameraManager->SetActiveCamera( cameraComponent );
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
		AddActorLocation( GetActorForwardVector() * -2.f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyW ) )
	{
		AddActorLocation( GetActorForwardVector() * 2.f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyA ) )
	{
		AddActorLocation( GetActorRightVector() * 2.f );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyD ) )
	{
		AddActorLocation( GetActorRightVector() * -2.f );
	}
}
