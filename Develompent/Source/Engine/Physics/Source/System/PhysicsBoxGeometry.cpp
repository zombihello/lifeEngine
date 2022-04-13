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
	handle = FPhysicsInterface::CreateBoxGeometry( extent, material->GetMaterialHandle() );
}

void FPhysicsBoxGeometry::Serialize( class FArchive& InArchive )
{
	FPhysicsShapeGeometry::Serialize( InArchive );
	InArchive << center;
	InArchive << rotation;
	InArchive << extent;
}