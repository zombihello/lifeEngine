#include "Components/BoxComponent.h"

IMPLEMENT_CLASS( LBoxComponent )

LBoxComponent::LBoxComponent()
	: size( 1.f, 1.f, 1.f )
{
	SetCollision( true );
}

void LBoxComponent::UpdateBodySetup()
{
	bodySetup = new FPhysicsBodySetup();
	bodySetup->AddBoxGeometry( FPhysicsBoxGeometry( size.x, size.y, size.z ) );
}

void LBoxComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << size;
}