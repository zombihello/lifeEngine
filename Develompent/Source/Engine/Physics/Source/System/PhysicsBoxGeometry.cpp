#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBoxGeometry.h"

void FPhysicsBoxGeometry::InitPhysXShape()
{
	if ( pxShape )
	{
		return;
	}

	check( material );
	pxShape = GPhysicsEngine.GetPxPhysics()->createShape( physx::PxBoxGeometry( extent.x, extent.y, extent.z ), *material->GetPhysXMaterial() );
	check( pxShape );
}

void FPhysicsBoxGeometry::Serialize( class FArchive& InArchive )
{
	FPhysicsShapeGeometry::Serialize( InArchive );
	InArchive << center;
	InArchive << rotation;
	InArchive << extent;
}