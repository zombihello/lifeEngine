#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsShapeGeometry.h"

/*
==================
SPhysicsShapeGeometry::SPhysicsShapeGeometry
==================
*/
SPhysicsShapeGeometry::SPhysicsShapeGeometry( ECollisionShape InCollisionShape )
	: collisionShape( InCollisionShape )
	, collisionProfile( g_PhysicsEngine.FindCollisionProfile( SCollisionProfile::blockAll_ProfileName ) )
	, material( g_PhysicsEngine.GetDefaultPhysMaterial() )
{}

/*
==================
SPhysicsShapeGeometry::~SPhysicsShapeGeometry
==================
*/
SPhysicsShapeGeometry::~SPhysicsShapeGeometry()
{
	CPhysicsInterface::ReleaseShapeGeometry( handle );
}

/*
==================
SPhysicsShapeGeometry::Serialize
==================
*/
void SPhysicsShapeGeometry::Serialize( class CArchive& InArchive )
{
	InArchive << collisionShape;
	InArchive << collisionProfile;
	InArchive << material;
}