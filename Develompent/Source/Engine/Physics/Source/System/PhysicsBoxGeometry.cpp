#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBoxGeometry.h"

void SPhysicsBoxGeometry::InitPhysicsShape() const
{
	if ( CPhysicsInterface::IsValidShapeGeometry( handle ) )
	{
		return;
	}

	check( material.IsAssetValid() );
	handle = CPhysicsInterface::CreateShapeGeometry( *this );
}

void SPhysicsBoxGeometry::Serialize( class CArchive& InArchive )
{
	SPhysicsShapeGeometry::Serialize( InArchive );
	InArchive << location;
	InArchive << rotation;
	InArchive << extent;
}