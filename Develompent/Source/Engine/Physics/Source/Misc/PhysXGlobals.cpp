#if WITH_PHYSX
#include "Misc/PhysXGlobals.h"
#include "System/PhysXScene.h"

// -------------
// GLOBALS
// -------------

physx::PxFoundation*			GPhysXFoundation		= nullptr;
physx::PxPhysics*				GPhysXSDK				= nullptr;
physx::PxAllocatorCallback*		GPhysXAllocator			= nullptr;
physx::PxErrorCallback*			GPhysXError				= nullptr;

#if !SHIPPING_BUILD
physx::PxPvd*					GPhysXVisualDebugger	= nullptr;
#endif // !SHIPPING_BUILD

CPhysXScene						GPhysicsScene;
#endif // WITH_PHYSX