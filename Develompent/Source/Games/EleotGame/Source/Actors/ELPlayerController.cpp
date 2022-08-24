#include "Actors/ELPlayerController.h"

IMPLEMENT_CLASS( AELPlayerController )

void AELPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released,	std::bind( &AELPlayerController::ExitFromGame, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Pressed,		std::bind( &AELPlayerController::Jump, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Released,	std::bind( &AELPlayerController::StopJump, this ) );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveRight" ),				std::bind( &AELPlayerController::MoveRight, this, std::placeholders::_1 ) );
}

void AELPlayerController::ExitFromGame()
{
	GIsRequestingExit = true;
}

void AELPlayerController::MoveRight( float InValue )
{
	character->Walk( character->GetActorRightVector(), InValue );
}

void AELPlayerController::Jump()
{
	character->Jump();
}

void AELPlayerController::StopJump()
{
	character->StopJump();
}