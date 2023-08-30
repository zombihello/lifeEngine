#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( CShapeComponent )

/*
==================
CShapeComponent::CShapeComponent
==================
*/
CShapeComponent::CShapeComponent()
	: collisionProfile( g_PhysicsEngine.FindCollisionProfile( CollisionProfile::blockAll_ProfileName ) )
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
CShapeComponent::StaticInitializeClass
==================
*/
void CShapeComponent::StaticInitializeClass()
{
	// TODO BS yehor.pohuliaka - Need implemente CStructProperty for user structs
	new CAssetProperty( staticClass, TEXT( "Physics Material" ), TEXT( "Physics" ), TEXT( "Physics material" ), CPP_PROPERTY( physicsMaterial ), 0, AT_PhysicsMaterial );
}

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