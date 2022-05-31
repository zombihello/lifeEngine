#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsShapeGeometry.h"

SPhysicsShapeGeometry::SPhysicsShapeGeometry( ECollisionShape InCollisionShape )
	: collisionShape( InCollisionShape )
	, collisionProfile( GPhysicsEngine.FindCollisionProfile( SCollisionProfile::blockAll_ProfileName ) )
	, material( GPhysicsEngine.GetDefaultPhysMaterial() )
{}

SPhysicsShapeGeometry::~SPhysicsShapeGeometry()
{
	CPhysicsInterface::ReleaseShapeGeometry( handle );
}

void SPhysicsShapeGeometry::Serialize( class CArchive& InArchive )
{
	InArchive << collisionShape;
	InArchive << collisionProfile;
	InArchive << material;
}