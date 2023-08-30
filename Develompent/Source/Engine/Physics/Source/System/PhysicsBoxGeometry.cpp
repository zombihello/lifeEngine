#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBoxGeometry.h"

/*
==================
PhysicsBoxGeometry::InitPhysicsShape
==================
*/
void PhysicsBoxGeometry::InitPhysicsShape() const
{
	if ( CPhysicsInterface::IsValidShapeGeometry( handle ) )
	{
		return;
	}

	Assert( material.IsAssetValid() );
	handle = CPhysicsInterface::CreateShapeGeometry( *this );
}

/*
==================
PhysicsBoxGeometry::Serialize
==================
*/
void PhysicsBoxGeometry::Serialize( class CArchive& InArchive )
{
	PhysicsShapeGeometry::Serialize( InArchive );
	InArchive << location;
	InArchive << rotation;
	InArchive << extent;
}