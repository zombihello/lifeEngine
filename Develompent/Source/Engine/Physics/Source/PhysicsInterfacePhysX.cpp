#if WITH_PHYSX
#include "Logger/LoggerMacros.h"
#include "System/PhysicsMaterial.h"
#include "PhysicsInterfacePhysX.h"

/*
==================
SPhysicsInterfacePhysX::Init
==================
*/
void SPhysicsInterfacePhysX::Init()
{
	Logf( TEXT( "PhysX version: %i.%i.%i\n" ), PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX );

	g_PhysXAllocator		= new physx::PxDefaultAllocator();
	g_PhysXError			= new physx::PxDefaultErrorCallback();

	// Create PhysX foundation
	g_PhysXFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *g_PhysXAllocator, *g_PhysXError );
	Assert( g_PhysXFoundation );

	// Create profile manager
#if !SHIPPING_BUILD
	g_PhysXVisualDebugger = PxCreatePvd( *g_PhysXFoundation );
	physx::PxPvdTransport*		pxPvdTransport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
	Assert( g_PhysXVisualDebugger && pxPvdTransport );
	g_PhysXVisualDebugger->connect( *pxPvdTransport, physx::PxPvdInstrumentationFlag::eALL );
#endif // !SHIPPING_BUILD

	// Create physics
	g_PhysXSDK = PxCreatePhysics( PX_PHYSICS_VERSION, *g_PhysXFoundation, physx::PxTolerancesScale(), false
#if !SHIPPING_BUILD
								 , g_PhysXVisualDebugger
#endif // !SHIPPING_BUILD
	);
	Assert( g_PhysXSDK );
}

/*
==================
SPhysicsInterfacePhysX::Shutdown
==================
*/
void SPhysicsInterfacePhysX::Shutdown()
{
	if ( g_PhysXSDK )
	{
		g_PhysXSDK->release();
		g_PhysXSDK = nullptr;
	}

	if ( g_PhysXVisualDebugger )
	{
		g_PhysXVisualDebugger->release();
		g_PhysXVisualDebugger = nullptr;
	}

	if ( g_PhysXFoundation )
	{
		g_PhysXFoundation->release();
		g_PhysXFoundation = nullptr;
	}

	if ( g_PhysXAllocator )
	{
		delete g_PhysXAllocator;
		g_PhysXAllocator = nullptr;
	}

	if ( g_PhysXError )
	{
		delete g_PhysXError;
		g_PhysXError = nullptr;
	}
}

/*
==================
SPhysicsInterfacePhysX::CreateMaterial
==================
*/
SPhysicsMaterialHandlePhysX SPhysicsInterfacePhysX::CreateMaterial( class CPhysicsMaterial* InPhysMaterial )
{
	Assert( InPhysMaterial );

	SPhysicsMaterialHandlePhysX		materialHandle;
	materialHandle.pxMaterial = g_PhysXSDK->createMaterial( InPhysMaterial->GetStaticFriction(), InPhysMaterial->GetDynamicFriction(), InPhysMaterial->GetRestitution() );
	return materialHandle;
}

/*
==================
SPhysicsInterfacePhysX::UpdateMaterial
==================
*/
void SPhysicsInterfacePhysX::UpdateMaterial( const SPhysicsMaterialHandlePhysX& InMaterialHandle, class CPhysicsMaterial* InPhysMaterial )
{
	Assert( SPhysicsInterfacePhysX::IsValidMaterial( InMaterialHandle ) && InPhysMaterial );
	InMaterialHandle.pxMaterial->setStaticFriction( InPhysMaterial->GetStaticFriction() );
	InMaterialHandle.pxMaterial->setDynamicFriction( InPhysMaterial->GetDynamicFriction() );
	InMaterialHandle.pxMaterial->setRestitution( InPhysMaterial->GetRestitution() );
}

/*
==================
SPhysicsInterfacePhysX::CreateActor
==================
*/
SPhysicsActorHandlePhysX SPhysicsInterfacePhysX::CreateActor( const SActorCreationParams& InParams )
{
	PhysicsActorHandle_t		actorHandle;
	physx::PxTransform		pxTransform = LE2PTransform( InParams.initialTM );
	actorHandle.bStatic		= InParams.bStatic;

	// Create static
	if ( InParams.bStatic )
	{
		actorHandle.pxRigidActor = g_PhysXSDK->createRigidStatic( pxTransform );
		actorHandle.pxRigidActor->setName( InParams.debugName );
	}
	// Create dynamic
	else
	{
		physx::PxRigidDynamic*		pxRigidDynamic = g_PhysXSDK->createRigidDynamic( pxTransform );
		actorHandle.pxRigidActor = pxRigidDynamic;

		pxRigidDynamic->setName( InParams.debugName );
		pxRigidDynamic->setRigidBodyFlag( physx::PxRigidBodyFlag::eUSE_KINEMATIC_TARGET_FOR_SCENE_QUERIES, true );
		pxRigidDynamic->setRigidDynamicLockFlags( LE2PLockFlags( InParams.lockFlags ) );
		pxRigidDynamic->setMass( InParams.mass );
		
		if ( !InParams.bEnableGravity )
		{
			actorHandle.pxRigidActor->setActorFlag( physx::PxActorFlag::eDISABLE_GRAVITY, true );
		}
	}

	if ( InParams.bQueryOnly )
	{
		actorHandle.pxRigidActor->setActorFlag( physx::PxActorFlag::eDISABLE_SIMULATION, true );
	}
	return actorHandle;
}

#endif // WITH_PHYSX