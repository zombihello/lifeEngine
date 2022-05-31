#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( CShapeComponent )

CShapeComponent::CShapeComponent()
	: collisionProfile( GPhysicsEngine.FindCollisionProfile( SCollisionProfile::blockAll_ProfileName ) )
	, physicsMaterial( nullptr )
{
	SetVisibility( false );
}

CShapeComponent::~CShapeComponent()
{}

void CShapeComponent::BeginPlay()
{
	Super::BeginPlay();
	if ( !physicsMaterial.IsAssetValid() )
	{
		physicsMaterial = GPhysicsEngine.GetDefaultPhysMaterial();
	}

	UpdateBodySetup();
}

void CShapeComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << collisionProfile;
	InArchive << physicsMaterial;
}