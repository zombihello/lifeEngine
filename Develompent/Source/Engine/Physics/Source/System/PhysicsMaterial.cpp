#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsMaterial.h"

FPhysicsMaterial::FPhysicsMaterial()
	: FAsset( AT_PhysicsMaterial )
	, staticFriction( 0.f )
	, dynamicFriction( 0.f )
	, restitution( 0.f )
	, density( 0.f )
	, surfaceType( ST_Default )
{}

FPhysicsMaterial::~FPhysicsMaterial()
{
	onPhysicsMaterialDestroyed.Broadcast( SharedThis( this ) );
	FPhysicsInterface::ReleaseMaterial( handle );
}

void FPhysicsMaterial::Serialize( class FArchive& InArchive )
{
	FAsset::Serialize( InArchive );
	InArchive << staticFriction;
	InArchive << dynamicFriction;
	InArchive << restitution;
	InArchive << density;
	InArchive << surfaceType;

	if ( InArchive.IsLoading() )
	{
		FPhysicsInterface::UpdateMaterial( handle, SharedThis( this ) );
	}
}