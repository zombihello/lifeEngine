#include "Actors/ELPlayerController.h"
#include "Actors/Character.h"

IMPLEMENT_CLASS( AELPlayerController )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( AELPlayerController )

/*
==================
AELPlayerController::AELPlayerController
==================
*/
AELPlayerController::AELPlayerController()
{
	bShowMouseCursor	= false;
	bConstrainYaw		= true;
}

/*
==================
AELPlayerController::SetupInputComponent
==================
*/
void AELPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released,	std::bind( &AELPlayerController::ExitFromGame, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Pressed,		std::bind( &AELPlayerController::Jump, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Released,	std::bind( &AELPlayerController::StopJump, this ) );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveForward" ),	std::bind( &AELPlayerController::MoveForward, this, std::placeholders::_1	) );
	inputComponent->BindAxis( TEXT( "MoveRight" ),		std::bind( &AELPlayerController::MoveRight, this, std::placeholders::_1		) );
	inputComponent->BindAxis( TEXT( "Turn" ),			std::bind( &AELPlayerController::Turn, this, std::placeholders::_1			) );
	inputComponent->BindAxis( TEXT( "LookUp" ),			std::bind( &AELPlayerController::LookUp, this, std::placeholders::_1		) );
}

/*
==================
AELPlayerController::ExitFromGame
==================
*/
void AELPlayerController::ExitFromGame()
{
	g_IsRequestingExit = true;
}

/*
==================
AELPlayerController::MoveForward
==================
*/
void AELPlayerController::MoveForward( float InValue )
{
	character->Walk( character->GetActorForwardVector(), InValue );
}

/*
==================
AELPlayerController::MoveRight
==================
*/
void AELPlayerController::MoveRight( float InValue )
{
	character->Walk( character->GetActorRightVector(), InValue );
}

/*
==================
AELPlayerController::Turn
==================
*/
void AELPlayerController::Turn( float InValue )
{
	AddYawInput( InValue );
}

/*
==================
AELPlayerController::LookUp
==================
*/
void AELPlayerController::LookUp( float InValue )
{
	AddPitchInput( InValue );
}

/*
==================
AELPlayerController::Jump
==================
*/
void AELPlayerController::Jump()
{
	character->Jump();
}

/*
==================
AELPlayerController::StopJump
==================
*/
void AELPlayerController::StopJump()
{
	character->StopJump();
}