#if WITH_PHYSX
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "System/PhysXScene.h"

CPhysXScene::CPhysXScene()
	: pxScene( nullptr )
	, pxDefaultCpuDispatcher( nullptr )
{}

CPhysXScene::~CPhysXScene()
{
	Shutdown();
}

void CPhysXScene::Init()
{
	// Create CPU dispatcher
	pxDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate( 2 );
	
	// Create PhysX scene
	physx::PxSceneDesc			pxSceneDescriptor( GPhysXSDK->getTolerancesScale() );
	pxSceneDescriptor.gravity			= physx::PxVec3( 0.0f, -9.81f, 0.0f );	
	pxSceneDescriptor.cpuDispatcher		= pxDefaultCpuDispatcher;
	pxSceneDescriptor.filterShader		= physx::PxDefaultSimulationFilterShader;
	pxScene = GPhysXSDK->createScene( pxSceneDescriptor );
	check( pxScene );
}

void CPhysXScene::Tick( float InDeltaTime )
{
	pxScene->simulate( InDeltaTime );
	pxScene->fetchResults( true );
}

void CPhysXScene::Shutdown()
{
	// Free allocated memory
	if ( pxScene )
	{
		pxScene->release();
		pxScene = nullptr;
	}

	if ( pxDefaultCpuDispatcher )
	{
		pxDefaultCpuDispatcher->release();
		pxDefaultCpuDispatcher = nullptr;
	}
}

void CPhysXScene::AddBody( class CPhysicsBodyInstance* InBodyInstance )
{
	physx::PxRigidActor*		pxRigidBody = InBodyInstance->GetActorHandle().pxRigidActor;
	check( pxRigidBody );

	pxScene->addActor( *pxRigidBody );
	bodies.push_back( InBodyInstance );
}

void CPhysXScene::RemoveBody( CPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		CPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			physx::PxRigidActor*		pxRigidBody = bodyInstance->GetActorHandle().pxRigidActor;
			check( pxRigidBody );

			pxScene->removeActor( *pxRigidBody );
			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}

void CPhysXScene::RemoveAllBodies()
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		CPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		physx::PxRigidActor*		pxRigidBody = bodyInstance->GetActorHandle().pxRigidActor;
		check( pxRigidBody );

		pxScene->removeActor( *pxRigidBody );
	}
	bodies.clear();
}
#endif // WITH_PHYSX