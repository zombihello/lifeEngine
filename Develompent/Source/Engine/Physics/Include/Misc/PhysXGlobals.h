/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSXGLOBALS_H
#define PHYSXGLOBALS_H

#if WITH_PHYSX
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

/**
 * @ingroup Physics
 * PhysX foundation
 */
extern physx::PxFoundation*				GPhysXFoundation;

/**
 * @ingroup Physics
 * PhysX SDK
 */
extern physx::PxPhysics*				GPhysXSDK;

/**
 * @ingroup Physics
 * PhysX allocator callback
 */
extern physx::PxAllocatorCallback*		GPhysXAllocator;

/**
 * @ingroup Physics
 * PhysX error callback
 */
extern physx::PxErrorCallback*			GPhysXError;

#if !SHIPPING_BUILD
/**
 * @ingroup Physics
 * PhysX visual debugger
 */
extern physx::PxPvd*					GPhysXVisualDebugger;
#endif // !SHIPPING_BUILD

/**
 * @ingroup Physics
 * Physics scene
 */
extern class CPhysXScene				GPhysicsScene;
#endif // WITH_PHYSX

#endif // !PHYSXGLOBALS_H