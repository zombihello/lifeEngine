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

typedef SPhysicsInterfacePhysX					CPhysicsInterface;
typedef SPhysicsMaterialHandlePhysX				PhysicsMaterialHandle_t;
typedef SPhysicsShapeHandlePhysX				PhysicsShapeHandle_t;
typedef SPhysicsActorHandlePhysX				PhysicsActorHandle_t;

// Box 2D implementation
#elif WITH_BOX2D
#include "PhysicsInterfaceBox2D.h"
#include "Misc/Box2DGlobals.h"
#include "System/Box2DScene.h"

typedef SPhysicsInterfaceBox2D					CPhysicsInterface;
typedef SPhysicsMaterialHandleBox2D				PhysicsMaterialHandle_t;
typedef SPhysicsShapeHandleBox2D				PhysicsShapeHandle_t;
typedef SPhysicsActorHandleBox2D				PhysicsActorHandle_t;

#else
	static_assert( false, "A physics engine interface must be defined to build" );
#endif // !WITH_PHYSX

#endif // !PHYSICSINTERFACE_H