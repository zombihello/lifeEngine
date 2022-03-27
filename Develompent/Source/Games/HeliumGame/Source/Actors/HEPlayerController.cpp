#include "Actors/HEPlayerController.h"

IMPLEMENT_CLASS( AHEPlayerController )

void AHEPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released, this, &AHEPlayerController::ExitFromGame );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveRight" ), this, &AHEPlayerController::MoveRight );
	inputComponent->BindAxis( TEXT( "MoveUp" ), this, &AHEPlayerController::MoveUp );
}

void AHEPlayerController::ExitFromGame()
{
	GIsRequestingExit = true;
}

void AHEPlayerController::MoveRight( float InValue )
{
	character->AddActorLocation( character->GetActorRightVector() * InValue );
}

void AHEPlayerController::MoveUp( float InValue )
{
	character->AddActorLocation( character->GetActorUpVector() * InValue );
}