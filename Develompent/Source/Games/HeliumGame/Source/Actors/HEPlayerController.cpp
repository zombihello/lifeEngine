#include "Actors/HEPlayerController.h"

IMPLEMENT_CLASS( AHEPlayerController )

void AHEPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released,	std::bind( &AHEPlayerController::ExitFromGame, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Pressed,		std::bind( &AHEPlayerController::Jump, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Released,	std::bind( &AHEPlayerController::StopJump, this ) );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveRight" ),				std::bind( &AHEPlayerController::MoveRight, this, std::placeholders::_1 ) );
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