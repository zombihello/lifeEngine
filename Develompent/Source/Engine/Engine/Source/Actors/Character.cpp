#include "Actors/Character.h"

IMPLEMENT_CLASS( ACharacter )

/*
==================
ACharacter::ACharacter
==================
*/
ACharacter::ACharacter()
{
	characterMovement = CreateComponent<CCharacterMovementComponent>( TEXT( "CharacterMovementComponent0" ) );
}

/*
==================
ACharacter::StaticInitializeClass
==================
*/
void ACharacter::StaticInitializeClass()
{
	// Native properties
	new( staticClass, TEXT( "Character Movement" ) ) CObjectProperty( CPP_PROPERTY( ThisClass, characterMovement ), TEXT( "Character" ), TEXT( "Character movement component" ), CPF_Edit, CCharacterMovementComponent::StaticClass() );
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, controller ), NAME_None, TEXT( "" ), CPF_None, ABaseController::StaticClass() );
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