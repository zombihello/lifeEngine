#include "Actors/PlayerController.h"
#include "Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )

APlayerController::APlayerController()
{
	inputComponent = CreateComponent< CInputComponent >( TEXT( "InputComponent0" ) );
}

APlayerController::~APlayerController()
{}

void APlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
}

void APlayerController::SetupInputComponent()
{}