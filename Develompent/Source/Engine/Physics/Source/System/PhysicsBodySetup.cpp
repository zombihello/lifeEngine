#include "System/PhysicsBodySetup.h"

CPhysicsBodySetup::CPhysicsBodySetup()
{}

CPhysicsBodySetup::~CPhysicsBodySetup()
{
	RemoveAllBoxGeometries();
}

void CPhysicsBodySetup::Serialize( class CArchive& InArchive )
{
	InArchive << boxGeometries;
}

void CPhysicsBodySetup::RemoveBoxGeometry( const SPhysicsBoxGeometry& InBoxGeometry )
{
	for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
	{
		const SPhysicsBoxGeometry&		boxGeometry = boxGeometries[ index ];
		if ( boxGeometry == InBoxGeometry )
		{
			RemoveBoxGeometry( index );
			return;
		}
	}
}