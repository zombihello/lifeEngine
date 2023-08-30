/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSINTERFACE_H
#define PHYSICSINTERFACE_H

#ifndef WITH_PHYSX
	#define WITH_PHYSX	0
#endif // !WITH_PHYSX

#ifndef WITH_BOX2D
	#define WITH_BOX2D	0
#endif // !WITH_BOX2D

#include "PhysicsInterfaceTypes.h"

// PhysX implementation
#if WITH_PHYSX
#include "PhysicsInterfacePhysX.h"
#include "Misc/PhysXGlobals.h"
#include "System/PhysXScene.h"

typedef PhysicsInterfacePhysX				CPhysicsInterface;
typedef PhysicsMaterialHandlePhysX			PhysicsMaterialHandle_t;
typedef PhysicsShapeHandlePhysX				PhysicsShapeHandle_t;
typedef PhysicsActorHandlePhysX				PhysicsActorHandle_t;

// Box 2D implementation
#elif WITH_BOX2D
#include "PhysicsInterfaceBox2D.h"
#include "Misc/Box2DGlobals.h"
#include "System/Box2DScene.h"

typedef PhysicsInterfaceBox2D				CPhysicsInterface;
typedef PhysicsMaterialHandleBox2D			PhysicsMaterialHandle_t;
typedef PhysicsShapeHandleBox2D				PhysicsShapeHandle_t;
typedef PhysicsActorHandleBox2D				PhysicsActorHandle_t;

#else
	static_assert( false, "A physics engine interface must be defined to build" );
#endif // !WITH_PHYSX

#endif // !PHYSICSINTERFACE_H