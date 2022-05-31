#if WITH_PHYSX
#include "Logger/LoggerMacros.h"
#include "System/PhysicsMaterial.h"
#include "PhysicsInterfacePhysX.h"

void SPhysicsInterfacePhysX::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "PhysX version: %i.%i.%i" ), PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX );

	GPhysXAllocator		= new physx::PxDefaultAllocator();
	GPhysXError			= new physx::PxDefaultErrorCallback();

	// Create PhysX foundation
	GPhysXFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *GPhysXAllocator, *GPhysXError );
	check( GPhysXFoundation );

	// Create profile manager
#if !SHIPPING_BUILD
	GPhysXVisualDebugger = PxCreatePvd( *GPhysXFoundation );
	physx::PxPvdTransport*		pxPvdTransport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
	check( GPhysXVisualDebugger && pxPvdTransport );
	GPhysXVisualDebugger->connect( *pxPvdTransport, physx::PxPvdInstrumentationFlag::eALL );
#endif // !SHIPPING_BUILD

	// Create physics
	GPhysXSDK = PxCreatePhysics( PX_PHYSICS_VERSION, *GPhysXFoundation, physx::PxTolerancesScale(), false
#if !SHIPPING_BUILD
								 , GPhysXVisualDebugger
#endif // !SHIPPING_BUILD
	);
	check( GPhysXSDK );
}

void SPhysicsInterfacePhysX::Shutdown()
{
	if ( GPhysXSDK )
	{
		GPhysXSDK->release();
		GPhysXSDK = nullptr;
	}

	if ( GPhysXVisualDebugger )
	{
		GPhysXVisualDebugger->release();
		GPhysXVisualDebugger = nullptr;
	}

	if ( GPhysXFoundation )
	{
		GPhysXFoundation->release();
		GPhysXFoundation = nullptr;
	}

	if ( GPhysXAllocator )
	{
		delete GPhysXAllocator;
		GPhysXAllocator = nullptr;
	}

	if ( GPhysXError )
	{
		delete GPhysXError;
		GPhysXError = nullptr;
	}
}

SPhysicsMaterialHandlePhysX SPhysicsInterfacePhysX::CreateMaterial( class CPhysicsMaterial* InPhysMaterial )
{
	check( InPhysMaterial );

	SPhysicsMaterialHandlePhysX		materialHandle;
	materialHandle.pxMaterial = GPhysXSDK->createMaterial( InPhysMaterial->GetStaticFriction(), InPhysMaterial->GetDynamicFriction(), InPhysMaterial->GetRestitution() );
	return materialHandle;
}

void SPhysicsInterfacePhysX::UpdateMaterial( const SPhysicsMaterialHandlePhysX& InMaterialHandle, class CPhysicsMaterial* InPhysMaterial )
{
	check( SPhysicsInterfacePhysX::IsValidMaterial( InMaterialHandle ) && InPhysMaterial );
	InMaterialHandle.pxMaterial->setStaticFriction( InPhysMaterial->GetStaticFriction() );
	InMaterialHandle.pxMaterial->setDynamicFriction( InPhysMaterial->GetDynamicFriction() );
	InMaterialHandle.pxMaterial->setRestitution( InPhysMaterial->GetRestitution() );
}

SPhysicsActorHandlePhysX SPhysicsInterfacePhysX::CreateActor( const SActorCreationParams& InParams )
{
	PhysicsActorHandle_t		actorHandle;
	physx::PxTransform		pxTransform = LE2PTransform( InParams.initialTM );
	actorHandle.bStatic		= InParams.bStatic;

	// Create static
	if ( InParams.bStatic )
	{
		actorHandle.pxRigidActor = GPhysXSDK->createRigidStatic( pxTransform );
		actorHandle.pxRigidActor->setName( InParams.debugName );
	}
	// Create dynamic
	else
	{
		physx::PxRigidDynamic*		pxRigidDynamic = GPhysXSDK->createRigidDynamic( pxTransform );
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