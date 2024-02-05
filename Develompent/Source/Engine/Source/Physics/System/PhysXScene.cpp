/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#if WITH_PHYSX
#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"
#include "Physics/System/PhysicsBodyInstance.h"
#include "Physics/System/PhysXScene.h"

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