#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( CShapeComponent )

/*
==================
CShapeComponent::CShapeComponent
==================
*/
CShapeComponent::CShapeComponent()
	: collisionProfile( g_PhysicsEngine.FindCollisionProfile( SCollisionProfile::blockAll_ProfileName ) )
	, physicsMaterial( nullptr )
{
	SetVisibility( false );
}

/*
==================
CShapeComponent::~CShapeComponent
==================
*/
CShapeComponent::~CShapeComponent()
{}

/*
==================
CShapeComponent::BeginPlay
==================
*/
void CShapeComponent::BeginPlay()
{
	Super::BeginPlay();
	if ( !physicsMaterial.IsAssetValid() )
	{
		physicsMaterial = g_PhysicsEngine.GetDefaultPhysMaterial();
	}

	UpdateBodySetup();
}

/*
==================
CShapeComponent::Serialize
==================
*/
void CShapeComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << collisionProfile;
	InArchive << physicsMaterial;
}