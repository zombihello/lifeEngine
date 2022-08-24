#include "Actors/BaseController.h"
#include "Actors/Character.h"

IMPLEMENT_CLASS( ABaseController )

ABaseController::ABaseController()
{}

ABaseController::~ABaseController()
{}

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

TRefCountPtr<ACharacter> ABaseController::GetCharacter() const
{
	return character;
}