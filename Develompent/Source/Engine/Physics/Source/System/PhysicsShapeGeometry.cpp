#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsShapeGeometry.h"

/*
==================
PhysicsShapeGeometry::PhysicsShapeGeometry
==================
*/
PhysicsShapeGeometry::PhysicsShapeGeometry( ECollisionShape InCollisionShape )
	: collisionShape( InCollisionShape )
	, collisionProfile( g_PhysicsEngine.FindCollisionProfile( CollisionProfile::blockAll_ProfileName ) )
	, material( g_PhysicsEngine.GetDefaultPhysMaterial() )
{}

/*
==================
PhysicsShapeGeometry::~PhysicsShapeGeometry
==================
*/
PhysicsShapeGeometry::~PhysicsShapeGeometry()
{
	CPhysicsInterface::ReleaseShapeGeometry( handle );
}

/*
==================
PhysicsShapeGeometry::Serialize
==================
*/
void PhysicsShapeGeometry::Serialize( class CArchive& InArchive )
{
	InArchive << collisionShape;
	InArchive << collisionProfile;
	InArchive << material;
}