#include "System/PhysicsBodySetup.h"

FPhysicsBodySetup::FPhysicsBodySetup()
{}

FPhysicsBodySetup::~FPhysicsBodySetup()
{
	RemoveAllBoxGeometries();
}

void FPhysicsBodySetup::Serialize( class FArchive& InArchive )
{
	InArchive << boxGeometries;
}

void FPhysicsBodySetup::RemoveBoxGeometry( const FPhysicsBoxGeometry& InBoxGeometry )
{
	for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
	{
		const FPhysicsBoxGeometry&		boxGeometry = boxGeometries[ index ];
		if ( boxGeometry == InBoxGeometry )
		{
			RemoveBoxGeometry( index );
			return;
		}
	}
}