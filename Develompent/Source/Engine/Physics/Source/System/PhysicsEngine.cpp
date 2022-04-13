#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Config.h"
#include "System/PhysicsEngine.h"
#include "PhysicsInterface.h"

FPhysicsEngine::FPhysicsEngine()
{}

FPhysicsEngine::~FPhysicsEngine()
{
	Shutdown();
}

void FPhysicsEngine::Init()
{
	// Init physics interface
	FPhysicsInterface::Init();

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
	FPhysicsInterface::Shutdown();
}