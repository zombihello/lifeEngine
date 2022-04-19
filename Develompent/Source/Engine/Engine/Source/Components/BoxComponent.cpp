#include "Components/BoxComponent.h"

IMPLEMENT_CLASS( LBoxComponent )

LBoxComponent::LBoxComponent()
	: size( 1.f, 1.f, 1.f )
{
	bodyInstance.SetSimulatePhysics( true );
}

void LBoxComponent::UpdateBodySetup()
{
	bodySetup = new FPhysicsBodySetup();

	FPhysicsBoxGeometry		boxGeometry( size.x, size.y, size.z );
	boxGeometry.collisionProfile = collisionProfile;
	bodySetup->AddBoxGeometry( boxGeometry );
}

void LBoxComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << size;
}