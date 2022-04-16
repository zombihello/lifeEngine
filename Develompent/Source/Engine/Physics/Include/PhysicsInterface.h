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

typedef FPhysicsInterfacePhysX					FPhysicsInterface;
typedef FPhysicsMaterialHandlePhysX				FPhysicsMaterialHandle;
typedef FPhysicsShapeHandlePhysX				FPhysicsShapeHandle;
typedef FPhysicsActorHandlePhysX				FPhysicsActorHandle;

// Box 2D implementation
#elif WITH_BOX2D
#include "PhysicsInterfaceBox2D.h"
#include "Misc/Box2DGlobals.h"
#include "System/Box2DScene.h"

typedef FPhysicsInterfaceBox2D					FPhysicsInterface;
typedef FPhysicsMaterialHandleBox2D				FPhysicsMaterialHandle;
typedef FPhysicsShapeHandleBox2D				FPhysicsShapeHandle;
typedef FPhysicsActorHandleBox2D				FPhysicsActorHandle;

#else
	static_assert( false, "A physics engine interface must be defined to build" );
#endif // !WITH_PHYSX

#endif // !PHYSICSINTERFACE_H