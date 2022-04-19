#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( LShapeComponent )

LShapeComponent::LShapeComponent()
	: collisionProfile( GPhysicsEngine.FindCollisionProfile( FCollisionProfile::blockAll_ProfileName ) )
{
	SetVisibility( false );
}

LShapeComponent::~LShapeComponent()
{}

void LShapeComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateBodySetup();
}

void LShapeComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << collisionProfile;
}