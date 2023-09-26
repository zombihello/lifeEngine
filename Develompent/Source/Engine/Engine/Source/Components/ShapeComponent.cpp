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
	new CObjectProperty( staticClass, TEXT( "Collision Profile" ), TEXT( "Physics" ), TEXT( "Collision Profile" ), CPP_PROPERTY( collisionProfile ), CPF_Pointer | CPF_Const, CollisionProfile::StaticClass() );
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