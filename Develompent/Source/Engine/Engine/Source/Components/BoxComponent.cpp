#include "Components/BoxComponent.h"

IMPLEMENT_CLASS( LBoxComponent )

void LBoxComponent::UpdateBodySetup()
{
	bodySetup = new FPhysicsBodySetup();
	bodySetup->AddBoxGeometry( FPhysicsBoxGeometry( 20.f ) );
}