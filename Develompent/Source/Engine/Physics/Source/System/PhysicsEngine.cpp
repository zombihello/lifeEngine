#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Config.h"
#include "System/PhysicsScene.h"
#include "System/PhysicsEngine.h"

FPhysicsEngine::FPhysicsEngine()
	: pxFoundation( nullptr )
	, pxPhysics( nullptr )
#if !SHIPPING_BUILD
	, pxPvd( nullptr )
#endif // !SHIPPING_BUILD
{}

FPhysicsEngine::~FPhysicsEngine()
{}

void FPhysicsEngine::Init()
{
	LE_LOG( LT_Log, LC_Init, TEXT( "PhysX version: %i.%i.%i" ), PX_PHYSICS_VERSION_MAJOR, PX_PHYSICS_VERSION_MINOR, PX_PHYSICS_VERSION_BUGFIX );

	// Create PhysX foundation
	pxFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, pxDefaultAllocatorCallback, pxDefaultErrorCallback );
	check( pxFoundation );

	// Create PhysX PVD
#if !SHIPPING_BUILD
	pxPvd = PxCreatePvd( *pxFoundation );
	physx::PxPvdTransport*		pxPvdTransport = physx::PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
	check( pxPvd && pxPvdTransport );
	pxPvd->connect( *pxPvdTransport, physx::PxPvdInstrumentationFlag::eALL );
#endif // !SHIPPING_BUILD

	// Create PhysX core
	pxPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *pxFoundation, physx::PxTolerancesScale(), false
#if !SHIPPING_BUILD
								 , pxPvd
#endif // !SHIPPING_BUILD
	);
	check( pxPhysics );

	// Init physics scene
	GPhysicsScene.Init();

	// Load default physics material
	{
		defaultPhysMaterial = new FPhysicsMaterial();

		// Loading default material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			FConfigValue		configDefaultPhysMaterial = GEngineConfig.GetValue( TEXT( "Physics.Physics" ), TEXT( "DefaultPhysMaterial" ) );
			if ( configDefaultPhysMaterial.IsValid() )
			{
				std::wstring	pathAsset = configDefaultPhysMaterial.GetString();
				FAssetRef		asset = GPackageManager->FindAsset( pathAsset );
				if ( asset )
				{
					defaultPhysMaterial = asset;
				}
				else
				{
					LE_LOG( LT_Warning, LC_Init, TEXT( "Default physics material '%s' not loaded" ), pathAsset.c_str() );
				}
			}
			else
			{
				LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Engine' default physics material in section 'Physics.Physics:DefaultPhysMaterial'" ) );
			}
		}
	}
}

void FPhysicsEngine::Tick( float InDeltaTime )
{
	GPhysicsScene.Tick( InDeltaTime );
}

void FPhysicsEngine::Shutdown()
{
	// Free allocated memory
	GPhysicsScene.Shutdown();
	defaultPhysMaterial.SafeRelease();

	if ( pxPhysics )
	{
		pxPhysics->release();
		pxPhysics = nullptr;
	}

	if ( pxFoundation )
	{
		pxFoundation->release();
		pxFoundation = nullptr;
	}
}