#if WITH_PHYSX
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "System/PhysXScene.h"

/*
==================
CPhysXScene::CPhysXScene
==================
*/
CPhysXScene::CPhysXScene()
	: pxScene( nullptr )
	, pxDefaultCpuDispatcher( nullptr )
{}

/*
==================
CPhysXScene::~CPhysXScene
==================
*/
CPhysXScene::~CPhysXScene()
{
	Shutdown();
}

/*
==================
CPhysXScene::Init
==================
*/
void CPhysXScene::Init()
{
	// Create CPU dispatcher
	pxDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate( 2 );
	
	// Create PhysX scene
	physx::PxSceneDesc			pxSceneDescriptor( g_PhysXSDK->getTolerancesScale() );
	pxSceneDescriptor.gravity			= physx::PxVec3( 0.0f, -9.81f, 0.0f );	
	pxSceneDescriptor.cpuDispatcher		= pxDefaultCpuDispatcher;
	pxSceneDescriptor.filterShader		= physx::PxDefaultSimulationFilterShader;
	pxScene = g_PhysXSDK->createScene( pxSceneDescriptor );
	Assert( pxScene );
}

/*
==================
CPhysXScene::Tick
==================
*/
void CPhysXScene::Tick( float InDeltaTime )
{
	pxScene->simulate( InDeltaTime );
	pxScene->fetchResults( true );
}

/*
==================
CPhysXScene::Shutdown
==================
*/
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

/*
==================
CPhysXScene::AddBody
==================
*/
void CPhysXScene::AddBody( class CPhysicsBodyInstance* InBodyInstance )
{
	physx::PxRigidActor*		pxRigidBody = InBodyInstance->GetActorHandle().pxRigidActor;
	Assert( pxRigidBody );

	pxScene->addActor( *pxRigidBody );
	bodies.push_back( InBodyInstance );
}

/*
==================
CPhysXScene::RemoveBody
==================
*/
void CPhysXScene::RemoveBody( CPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		CPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			physx::PxRigidActor*		pxRigidBody = bodyInstance->GetActorHandle().pxRigidActor;
			Assert( pxRigidBody );

			pxScene->removeActor( *pxRigidBody );
			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}

/*
==================
CPhysXScene::RemoveAllBodies
==================
*/
void CPhysXScene::RemoveAllBodies()
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		CPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		physx::PxRigidActor*		pxRigidBody = bodyInstance->GetActorHandle().pxRigidActor;
		Assert( pxRigidBody );

		pxScene->removeActor( *pxRigidBody );
	}
	bodies.clear();
}
#endif // WITH_PHYSX