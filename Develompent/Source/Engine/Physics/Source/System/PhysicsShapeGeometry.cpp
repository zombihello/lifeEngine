#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsShapeGeometry.h"

FPhysicsShapeGeometry::FPhysicsShapeGeometry( ECollisionShape InCollisionShape )
	: collisionShape( InCollisionShape )
	, pxShape( nullptr )
	, material( GPhysicsEngine.GetDefaultPhysMaterial() )
{}

FPhysicsShapeGeometry::~FPhysicsShapeGeometry()
{
	if ( pxShape )
	{
		pxShape->release();
		pxShape = nullptr;
	}
}

void FPhysicsShapeGeometry::Serialize( class FArchive& InArchive )
{
	InArchive << collisionShape;
	InArchive << material;
}