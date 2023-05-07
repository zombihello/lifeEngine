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
extern physx::PxFoundation*				g_PhysXFoundation;

/**
 * @ingroup Physics
 * PhysX SDK
 */
extern physx::PxPhysics*				g_PhysXSDK;

/**
 * @ingroup Physics
 * PhysX allocator callback
 */
extern physx::PxAllocatorCallback*		g_PhysXAllocator;

/**
 * @ingroup Physics
 * PhysX error callback
 */
extern physx::PxErrorCallback*			g_PhysXError;

#if !SHIPPING_BUILD
/**
 * @ingroup Physics
 * PhysX visual debugger
 */
extern physx::PxPvd*					g_PhysXVisualDebugger;
#endif // !SHIPPING_BUILD

/**
 * @ingroup Physics
 * Physics scene
 */
extern class CPhysXScene				g_PhysicsScene;
#endif // WITH_PHYSX

#endif // !PHYSXGLOBALS_H