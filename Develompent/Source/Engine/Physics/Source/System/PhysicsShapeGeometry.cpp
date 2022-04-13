#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsShapeGeometry.h"

FPhysicsShapeGeometry::FPhysicsShapeGeometry( ECollisionShape InCollisionShape )
	: collisionShape( InCollisionShape )
	, material( GPhysicsEngine.GetDefaultPhysMaterial() )
{}

FPhysicsShapeGeometry::~FPhysicsShapeGeometry()
{
	FPhysicsInterface::ReleaseShapeGeometry( handle );
}

void FPhysicsShapeGeometry::Serialize( class FArchive& InArchive )
{
	InArchive << collisionShape;
	InArchive << material;
}