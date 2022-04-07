#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsMaterial.h"

FPhysicsMaterial::FPhysicsMaterial()
	: FAsset( AT_PhysicsMaterial )
	, staticFriction( 0.f )
	, dynamicFriction( 0.f )
	, restitution( 0.f )
	, pxMaterial( nullptr )
{
	pxMaterial = GPhysicsEngine.GetPxPhysics()->createMaterial( staticFriction, dynamicFriction, restitution );
	check( pxMaterial );
}

FPhysicsMaterial::~FPhysicsMaterial()
{
	if ( pxMaterial )
	{
		pxMaterial->release();
		pxMaterial = nullptr;
	}
}

void FPhysicsMaterial::Serialize( class FArchive& InArchive )
{
	FAsset::Serialize( InArchive );
	InArchive << staticFriction;
	InArchive << dynamicFriction;
	InArchive << restitution;

	if ( InArchive.IsLoading() )
	{
		pxMaterial->setStaticFriction( staticFriction );
		pxMaterial->setDynamicFriction( dynamicFriction );
		pxMaterial->setRestitution( restitution );
	}
}