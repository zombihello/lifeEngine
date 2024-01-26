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
CShapeComponent::StaticInitializeClass
==================
*/
void CShapeComponent::StaticInitializeClass()
{
	new( staticClass, TEXT( "Physics Material" ), OBJECT_Public )		CAssetProperty( CPP_PROPERTY( ThisClass, physicsMaterial ), TEXT( "Physics" ), TEXT( "Physics material" ), CPF_Edit, AT_PhysicsMaterial );
	
	// TODO yehor.pohuliaka - Need improve collision profile to we can have global profiles and overridden
	// new( staticClass, TEXT( "Collision Profile" ), OBJECT_Public )		CStructProperty( CPP_PROPERTY( ThisClass, collisionProfile ), TEXT( "Physics" ), TEXT( "Collision profile" ), CPF_Edit, CollisionProfile::StaticStruct() );
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
}