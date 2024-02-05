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
#include "Core/Logger/LoggerMacros.h"
#include "Physics/System/PhysicsMaterial.h"
#include "Physics/PhysicsInterfacePhysX.h"

/*
==================
PhysicsInterfacePhysX::Init
==================
*/
void PhysicsInterfacePhysX::Init()
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
PhysicsInterfacePhysX::Shutdown
==================
*/
void PhysicsInterfacePhysX::Shutdown()
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
PhysicsInterfacePhysX::CreateMaterial
==================
*/
PhysicsMaterialHandlePhysX PhysicsInterfacePhysX::CreateMaterial( class CPhysicsMaterial* InPhysMaterial )
{
	Assert( InPhysMaterial );

	PhysicsMaterialHandlePhysX		materialHandle;
	materialHandle.pxMaterial = g_PhysXSDK->createMaterial( InPhysMaterial->GetStaticFriction(), InPhysMaterial->GetDynamicFriction(), InPhysMaterial->GetRestitution() );
	return materialHandle;
}

/*
==================
PhysicsInterfacePhysX::UpdateMaterial
==================
*/
void PhysicsInterfacePhysX::UpdateMaterial( const PhysicsMaterialHandlePhysX& InMaterialHandle, class CPhysicsMaterial* InPhysMaterial )
{
	Assert( PhysicsInterfacePhysX::IsValidMaterial( InMaterialHandle ) && InPhysMaterial );
	InMaterialHandle.pxMaterial->setStaticFriction( InPhysMaterial->GetStaticFriction() );
	InMaterialHandle.pxMaterial->setDynamicFriction( InPhysMaterial->GetDynamicFriction() );
	InMaterialHandle.pxMaterial->setRestitution( InPhysMaterial->GetRestitution() );
}

/*
==================
PhysicsInterfacePhysX::CreateActor
==================
*/
PhysicsActorHandlePhysX PhysicsInterfacePhysX::CreateActor( const ActorCreationParams& InParams )
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