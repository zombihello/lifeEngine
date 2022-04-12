#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsScene.h"
#include "System/PhysicsBodyInstance.h"
#include "Components/PrimitiveComponent.h"

FPhysicsBodyInstance::FPhysicsBodyInstance()
	: bIsStatic( true )
	, lockFlags( BLF_None )
	, mass( 1.f )
	, pxRigidBody( nullptr )
{}

FPhysicsBodyInstance::~FPhysicsBodyInstance()
{
	TermBody();
}

void FPhysicsBodyInstance::InitBody( FPhysicsBodySetup* InBodySetup, const FTransform& InTransform, LPrimitiveComponent* InPrimComp )
{
	// If body is inited - terminate body for reinit
	if ( pxRigidBody )
	{
		TermBody();
	}

	check( InBodySetup );
	ownerComponent	= InPrimComp;
	bodySetup		= InBodySetup;

	physx::PxTransform		pxTransform = LE2PTransform( InTransform );
	if ( bIsStatic )
	{
		pxRigidBody = GPhysicsEngine.GetPxPhysics()->createRigidStatic( pxTransform );
	}
	else
	{
		physx::PxRigidDynamic*		pxRigidDinamic = GPhysicsEngine.GetPxPhysics()->createRigidDynamic( pxTransform );
		pxRigidDinamic->setRigidDynamicLockFlags( LE2PLockFlags( lockFlags ) );
		pxRigidDinamic->setMass( mass );
		pxRigidBody = pxRigidDinamic;
	}
	check( pxRigidBody );

	// Attach all shapes in body setup to PhysX rigid body
	// Box shapes
	{
		std::vector< FPhysicsBoxGeometry >&		boxGeometries = bodySetup->GetBoxGeometries();
		for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
		{
			FPhysicsBoxGeometry&		boxGeometry = boxGeometries[ index ];
			if ( !boxGeometry.pxShape )
			{
				boxGeometry.InitPhysXShape();
			}

			pxRigidBody->attachShape( *boxGeometry.pxShape );
		}
	}

	// Update mass and inertia if rigid body is not static
	if ( !bIsStatic )
	{
		physx::PxRigidBodyExt::updateMassAndInertia( *( physx::PxRigidDynamic* )pxRigidBody, 10.0f );
	}

	// Add rigid body to PhysX scene
	GPhysicsScene.AddBody( this );
}

void FPhysicsBodyInstance::TermBody()
{
	if ( !pxRigidBody )
	{
		return;
	}

	// Remove from scene
	GPhysicsScene.RemoveBody( this );

	// Release resource
	pxRigidBody->release();
	pxRigidBody = nullptr;
	ownerComponent = nullptr;
	bodySetup = nullptr;
}