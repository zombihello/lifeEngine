#include "System/PhysicsBodySetup.h"

/*
==================
CPhysicsBodySetup::CPhysicsBodySetup
==================
*/
CPhysicsBodySetup::CPhysicsBodySetup()
{}

/*
==================
CPhysicsBodySetup::~CPhysicsBodySetup
==================
*/
CPhysicsBodySetup::~CPhysicsBodySetup()
{
	RemoveAllBoxGeometries();
}

/*
==================
CPhysicsBodySetup::Serialize
==================
*/
void CPhysicsBodySetup::Serialize( class CArchive& InArchive )
{
	InArchive << boxGeometries;
}

/*
==================
CPhysicsBodySetup::RemoveBoxGeometry
==================
*/
void CPhysicsBodySetup::RemoveBoxGeometry( const PhysicsBoxGeometry& InBoxGeometry )
{
	for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
	{
		const PhysicsBoxGeometry&		boxGeometry = boxGeometries[ index ];
		if ( boxGeometry == InBoxGeometry )
		{
			RemoveBoxGeometry( index );
			return;
		}
	}
}