#include "Actors/HEPlayerController.h"

IMPLEMENT_CLASS( AHEPlayerController )

void AHEPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released,	this, &AHEPlayerController::ExitFromGame );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Pressed,		this, &AHEPlayerController::Jump );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Released,	this, &AHEPlayerController::StopJump );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveRight" ), this, &AHEPlayerController::MoveRight );
}

void AHEPlayerController::ExitFromGame()
{
	GIsRequestingExit = true;
}

void AHEPlayerController::MoveRight( float InValue )
{
	character->Walk( character->GetActorRightVector(), InValue );
}

void AHEPlayerController::Jump()
{
	character->Jump();
}

void AHEPlayerController::StopJump()
{
	character->StopJump();
}