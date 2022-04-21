#include "Actors/Character.h"

IMPLEMENT_CLASS( ACharacter )

ACharacter::ACharacter()
{
	characterMovement = CreateComponent< LCharacterMovementComponent >( TEXT( "CharacterMovementComponent0" ) );
}

ACharacter::~ACharacter()
{}

void ACharacter::Walk( const FVector& InWorldDirection, float InScale )
{
	characterMovement->Walk( InWorldDirection, InScale );
}

void ACharacter::Jump()
{
	characterMovement->Jump();
}

void ACharacter::StopJump()
{
	characterMovement->StopJump();
}

void ACharacter::Landed()
{}