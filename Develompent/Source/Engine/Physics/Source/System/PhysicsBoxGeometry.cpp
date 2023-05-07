#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBoxGeometry.h"

/*
==================
SPhysicsBoxGeometry::InitPhysicsShape
==================
*/
void SPhysicsBoxGeometry::InitPhysicsShape() const
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
SPhysicsBoxGeometry::Serialize
==================
*/
void SPhysicsBoxGeometry::Serialize( class CArchive& InArchive )
{
	SPhysicsShapeGeometry::Serialize( InArchive );
	InArchive << location;
	InArchive << rotation;
	InArchive << extent;
}