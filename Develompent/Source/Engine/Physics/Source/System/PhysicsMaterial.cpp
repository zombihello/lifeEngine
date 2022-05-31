#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsMaterial.h"

CPhysicsMaterial::CPhysicsMaterial()
	: CAsset( AT_PhysicsMaterial )
	, staticFriction( 0.f )
	, dynamicFriction( 0.f )
	, restitution( 0.f )
	, density( 0.f )
	, surfaceType( ST_Default )
{}

CPhysicsMaterial::~CPhysicsMaterial()
{
	onPhysicsMaterialDestroyed.Broadcast( SharedThis( this ) );
	CPhysicsInterface::ReleaseMaterial( handle );
}

void CPhysicsMaterial::Serialize( class CArchive& InArchive )
{
	CAsset::Serialize( InArchive );
	InArchive << staticFriction;
	InArchive << dynamicFriction;
	InArchive << restitution;
	InArchive << density;
	InArchive << surfaceType;

	if ( InArchive.IsLoading() )
	{
		CPhysicsInterface::UpdateMaterial( handle, SharedThis( this ) );
	}
}