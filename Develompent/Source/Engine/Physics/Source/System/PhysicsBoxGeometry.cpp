#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBoxGeometry.h"

void FPhysicsBoxGeometry::InitPhysicsShape() const
{
	if ( FPhysicsInterface::IsValidShapeGeometry( handle ) )
	{
		return;
	}

	check( material );
	handle = FPhysicsInterface::CreateShapeGeometry( *this );
}

void FPhysicsBoxGeometry::Serialize( class FArchive& InArchive )
{
	FPhysicsShapeGeometry::Serialize( InArchive );
	InArchive << location;
	InArchive << rotation;
	InArchive << extent;
}