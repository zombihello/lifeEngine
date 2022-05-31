#include "Components/BoxComponent.h"

IMPLEMENT_CLASS( CBoxComponent )

CBoxComponent::CBoxComponent()
	: size( 1.f, 1.f, 1.f )
{
	bodyInstance.SetSimulatePhysics( true );
}

void CBoxComponent::UpdateBodySetup()
{
	bodySetup = new CPhysicsBodySetup();

	SPhysicsBoxGeometry				boxGeometry( size.x, size.y, size.z );
	boxGeometry.collisionProfile	= collisionProfile;
	boxGeometry.material			= physicsMaterial;
	bodySetup->AddBoxGeometry( boxGeometry );
}

void CBoxComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << size;
}