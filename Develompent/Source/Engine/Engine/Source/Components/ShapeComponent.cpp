#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( LShapeComponent )

LShapeComponent::LShapeComponent()
	: collisionProfile( GPhysicsEngine.FindCollisionProfile( FCollisionProfile::blockAll_ProfileName ) )
	, physicsMaterial( nullptr )
{
	SetVisibility( false );
}

LShapeComponent::~LShapeComponent()
{}

void LShapeComponent::BeginPlay()
{
	Super::BeginPlay();
	if ( !physicsMaterial.IsAssetValid() )
	{
		physicsMaterial = GPhysicsEngine.GetDefaultPhysMaterial();
	}

	UpdateBodySetup();
}

void LShapeComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << collisionProfile;
	InArchive << physicsMaterial;
}