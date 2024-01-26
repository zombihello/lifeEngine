#include "Actors/BaseController.h"
#include "Actors/Character.h"

IMPLEMENT_CLASS( ABaseController )

/*
==================
ABaseController::ABaseController
==================
*/
ABaseController::ABaseController()
{}

/*
==================
ABaseController::StaticInitializeClass
==================
*/
void ABaseController::StaticInitializeClass()
{
	// Native properties
	new( staticClass, NAME_None, OBJECT_Public ) CObjectProperty( CPP_PROPERTY( ThisClass, character ), NAME_None, TEXT( "" ), CPF_Transient, ACharacter::StaticClass() );
}

/*
==================
ABaseController::SetCharacter
==================
*/
void ABaseController::SetCharacter( class ACharacter* InCharacter )
{
	if ( character )
	{
		character->SetController( nullptr );
	}

	character = InCharacter;

	if ( character )
	{
		character->SetController( this );
	}
}

/*
==================
ABaseController::GetCharacter
==================
*/
ACharacter* ABaseController::GetCharacter() const
{
	return character;
}