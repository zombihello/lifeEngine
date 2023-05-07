#include "Components/BoxComponent.h"

IMPLEMENT_CLASS( CBoxComponent )

/*
==================
CBoxComponent::CBoxComponent
==================
*/
CBoxComponent::CBoxComponent()
	: size( 1.f, 1.f, 1.f )
{
	bodyInstance.SetSimulatePhysics( true );
}

/*
==================
CBoxComponent::UpdateBodySetup
==================
*/
void CBoxComponent::UpdateBodySetup()
{
	bodySetup = new CPhysicsBodySetup();

	SPhysicsBoxGeometry				boxGeometry( size.x, size.y, size.z );
	boxGeometry.collisionProfile	= collisionProfile;
	boxGeometry.material			= physicsMaterial;
	bodySetup->AddBoxGeometry( boxGeometry );
}

/*
==================
CBoxComponent::Serialize
==================
*/
void CBoxComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << size;
}