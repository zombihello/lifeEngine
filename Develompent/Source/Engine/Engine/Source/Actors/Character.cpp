#include "Actors/Character.h"

IMPLEMENT_CLASS( ACharacter )

ACharacter::ACharacter()
{
	characterMovement = CreateComponent< CCharacterMovementComponent >( TEXT( "CharacterMovementComponent0" ) );
}

ACharacter::~ACharacter()
{}

void ACharacter::Walk( const Vector& InWorldDirection, float InScale )
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