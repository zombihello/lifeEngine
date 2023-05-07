#if WITH_PHYSX
#include "Misc/PhysXGlobals.h"
#include "System/PhysXScene.h"

// -------------
// GLOBALS
// -------------

physx::PxFoundation*			g_PhysXFoundation		= nullptr;
physx::PxPhysics*				g_PhysXSDK				= nullptr;
physx::PxAllocatorCallback*		g_PhysXAllocator			= nullptr;
physx::PxErrorCallback*			g_PhysXError				= nullptr;

#if !SHIPPING_BUILD
physx::PxPvd*					g_PhysXVisualDebugger	= nullptr;
#endif // !SHIPPING_BUILD

CPhysXScene						g_PhysicsScene;
#endif // WITH_PHYSX