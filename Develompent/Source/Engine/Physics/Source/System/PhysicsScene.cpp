#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsScene.h"

FPhysicsScene::FPhysicsScene()
	: pxScene( nullptr )
	, pxDefaultCpuDispatcher( nullptr )
{}

FPhysicsScene::~FPhysicsScene()
{
	Shutdown();
}

void FPhysicsScene::Init()
{
	// Create CPU dispatcher
	pxDefaultCpuDispatcher = physx::PxDefaultCpuDispatcherCreate( 2 );
	
	// Create PhysX scene
	physx::PxSceneDesc			pxSceneDescriptor( GPhysicsEngine.GetPxPhysics()->getTolerancesScale() );
	pxSceneDescriptor.gravity			= physx::PxVec3( 0.0f, -9.81f, 0.0f );	
	pxSceneDescriptor.cpuDispatcher		= pxDefaultCpuDispatcher;
	pxSceneDescriptor.filterShader		= physx::PxDefaultSimulationFilterShader;
	pxScene = GPhysicsEngine.GetPxPhysics()->createScene( pxSceneDescriptor );
	check( pxScene );
}

void FPhysicsScene::Tick()
{
	pxScene->simulate( 1.f / 60.f );
	pxScene->fetchResults( true );
}

void FPhysicsScene::Shutdown()
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

void FPhysicsScene::RemoveBody( FPhysicsBodyInstance* InBodyInstance )
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		FPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		if ( bodyInstance == InBodyInstance )
		{
			physx::PxRigidActor*		pxRigidBody = bodyInstance->GetPhysXRigidBody();
			check( pxRigidBody );

			pxScene->removeActor( *pxRigidBody );
			bodies.erase( bodies.begin() + index );
			return;
		}
	}
}

void FPhysicsScene::RemoveAllBodies()
{
	for ( uint32 index = 0, count = bodies.size(); index < count; ++index )
	{
		FPhysicsBodyInstance*		bodyInstance = bodies[ index ];
		physx::PxRigidActor*		pxRigidBody = bodyInstance->GetPhysXRigidBody();
		check( pxRigidBody );

		pxScene->removeActor( *pxRigidBody );
	}
	bodies.clear();
}