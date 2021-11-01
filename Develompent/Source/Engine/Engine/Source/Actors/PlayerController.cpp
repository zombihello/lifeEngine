#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "System/BaseWindow.h"
#include "Actors/PlayerController.h"
#include "Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )

LCameraComponent* cameraComponent;

APlayerController::APlayerController() :
	cameraComponent( new LCameraComponent() )
{
	::cameraComponent = this->cameraComponent;
	AddOwnedComponent( cameraComponent );
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
		cameraComponent->MoveComponent( FVector( 0.f, 0.f, 0.1f ) );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyW ) )
	{
		cameraComponent->MoveComponent( FVector( 0.f, 0.f, -0.1f ) );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyA ) )
	{
		cameraComponent->MoveComponent( FVector( -0.1f, 0.f, 0.f ) );
	}
	if ( GInputSystem->IsKeyDown( BC_KeyD ) )
	{
		cameraComponent->MoveComponent( FVector( 0.1f, 0.f, 0.f ) );
	}
}