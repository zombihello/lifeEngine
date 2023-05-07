#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Config.h"
#include "System/PhysicsEngine.h"
#include "System/Package.h"
#include "PhysicsInterface.h"

/*
==================
TextToECollisionChannel
==================
*/
FORCEINLINE ECollisionChannel TextToECollisionChannel( const std::wstring& InStr )
{
	if ( InStr == TEXT( "WorldStatic" ) )
	{
		return CC_WorldStatic;
	}
	else if ( InStr == TEXT( "Visibility" ) )
	{
		return CC_Visibility;
	}
	else if ( InStr == TEXT( "Character" ) )
	{
		return CC_Character;
	}
	
	Sys_Errorf( TEXT( "Unknown collision channel %s" ), InStr.c_str() );
	return CC_Max;
}

/*
==================
TextToECollisionChannel
==================
*/
FORCEINLINE ECollisionResponse TextToECollisionResponse( const std::wstring& InStr )
{
	if ( InStr == TEXT( "Block" ) )
	{
		return CR_Block;
	}
	else if ( InStr == TEXT( "Overlap" ) )
	{
		return CR_Overlap;
	}
	else if ( InStr == TEXT( "Ignore" ) )
	{
		return CR_Ignore;
	}

	Sys_Errorf( TEXT( "Unknown collision response %s" ), InStr.c_str() );
	return CR_Max;
}


/*
==================
CPhysicsEngine::CPhysicsEngine
==================
*/
CPhysicsEngine::CPhysicsEngine()
{}

/*
==================
CPhysicsEngine::~CPhysicsEngine
==================
*/
CPhysicsEngine::~CPhysicsEngine()
{
	Shutdown();
}

/*
==================
CPhysicsEngine::Init
==================
*/
void CPhysicsEngine::Init()
{
	// Init physics interface
	CPhysicsInterface::Init();

	// Init physics scene
	g_PhysicsScene.Init();

	// Load default physics material
	{
		// Loading default material from packages only when we in game
		if ( !g_IsCooker && !g_IsCommandlet )
		{
			CConfigValue		configDefaultPhysMaterial = g_Config.GetValue( CT_Engine, TEXT( "Physics.Physics" ), TEXT( "DefaultPhysMaterial" ) );
			if ( configDefaultPhysMaterial.IsValid() )
			{
				std::wstring			pathAsset = configDefaultPhysMaterial.GetString();
				TAssetHandle<CAsset>	asset = g_PackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
				{
					defaultPhysMaterial = asset;
				}
				else
				{
					Warnf( TEXT( "Default physics material '%s' not loaded\n" ), pathAsset.c_str() );
				}
			}
			else
			{
				Warnf( TEXT( "Need set in config 'Engine' default physics material in section 'Physics.Physics:DefaultPhysMaterial'\n" ) );
			}
		}

		// If default physics material not loaded we create in virtual package
		if ( !defaultPhysMaterial.IsAssetValid() )
		{
			PackageRef_t					package = g_PackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CPhysicsMaterial>	physMaterial = MakeSharedPtr<CPhysicsMaterial>();

			defaultPhysMaterial		= physMaterial->GetAssetHandle();
			package->Add( defaultPhysMaterial );
		}
	}

	// Load collision profiles
	{
		CConfigValue		configCollisionProfiles = g_Config.GetValue( CT_Engine, TEXT( "Physics.Physics" ), TEXT( "CollisionProfiles" ) );
		if ( configCollisionProfiles.IsValid() )
		{
			Assert( configCollisionProfiles.GetType() == CConfigValue::T_Array );
			std::vector< CConfigValue >		arrayCollisionProfiles = configCollisionProfiles.GetArray();

			for ( uint32 index = 0, count = arrayCollisionProfiles.size(); index < count; ++index )
			{
				const CConfigValue&		configCollisionProfile = arrayCollisionProfiles[ index ];
				Assert( configCollisionProfile.GetType() == CConfigValue::T_Object );
				CConfigObject			objectCollisionProfile = configCollisionProfile.GetObject();

				SCollisionProfile		collisionProfile;
				collisionProfile.name = objectCollisionProfile.GetValue( TEXT( "Name" ) ).GetString();

				std::wstring		objectType = objectCollisionProfile.GetValue( TEXT( "ObjectType" ) ).GetString();
				collisionProfile.objectType = TextToECollisionChannel( objectType );

				std::vector< CConfigValue >		arrayCollisionResponses = objectCollisionProfile.GetValue( TEXT( "CollisionResponses" ) ).GetArray();
				for ( uint32 indexResponse = 0, countResponse = arrayCollisionResponses.size(); indexResponse < countResponse; ++indexResponse )
				{
					const CConfigValue&		configCollisionResponse = arrayCollisionResponses[ indexResponse ];
					Assert( configCollisionResponse.GetType() == CConfigValue::T_Object );
					CConfigObject			objectCollisionResponse = configCollisionResponse.GetObject();

					std::wstring			name = objectCollisionResponse.GetValue( TEXT( "Name" ) ).GetString();
					std::wstring			value = objectCollisionResponse.GetValue( TEXT( "Value" ) ).GetString();
					collisionProfile.responses[ TextToECollisionChannel( name ) ] = TextToECollisionResponse( value );
				}

				collisionProfiles.insert( std::make_pair( collisionProfile.name, collisionProfile ) );
			}
		}
		else
		{
			Warnf( TEXT( "Need set in config 'Engine'collision profiles in section 'Physics.Physics:CollisionProfiles'\n" ) );
		}
	}

	// Register default assets
	g_AssetFactory.SetDefault( defaultPhysMaterial, AT_PhysicsMaterial );
}

/*
==================
CPhysicsEngine::Tick
==================
*/
void CPhysicsEngine::Tick( float InDeltaTime )
{
	g_PhysicsScene.Tick( InDeltaTime );
}

/*
==================
CPhysicsEngine::Shutdown
==================
*/
void CPhysicsEngine::Shutdown()
{
	// Free allocated memory
	g_PhysicsScene.Shutdown();
	defaultPhysMaterial.Reset();
	CPhysicsInterface::Shutdown();
}