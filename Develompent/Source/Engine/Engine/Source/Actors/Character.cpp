#include "Actors/Character.h"

IMPLEMENT_CLASS( ACharacter )

/*
==================
ACharacter::ACharacter
==================
*/
ACharacter::ACharacter()
{
	characterMovement = CreateComponent< CCharacterMovementComponent >( TEXT( "CharacterMovementComponent0" ) );
}

/*
==================
ACharacter::~ACharacter
==================
*/
ACharacter::~ACharacter()
{}

/*
==================
ACharacter::StaticInitializeClass
==================
*/
void ACharacter::StaticInitializeClass()
{
	new CObjectProperty( staticClass, TEXT( "Character Movement" ), TEXT( "Character" ), TEXT( "Character movement component" ), CPP_PROPERTY( characterMovement ), CPF_Pointer, CCharacterMovementComponent::StaticClass() );
}

/*
==================
ACharacter::Walk
==================
*/
void ACharacter::Walk( const Vector& InWorldDirection, float InScale )
{
	characterMovement->Walk( InWorldDirection, InScale );
}

/*
==================
ACharacter::Jump
==================
*/
void ACharacter::Jump()
{
	characterMovement->Jump();
}

/*
==================
ACharacter::StopJump
==================
*/
void ACharacter::StopJump()
{
	characterMovement->StopJump();
}

/*
==================
ACharacter::Landed
==================
*/
void ACharacter::Landed()
{}