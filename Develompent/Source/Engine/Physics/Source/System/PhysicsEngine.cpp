#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/PhysicsGlobals.h"
#include "System/Config.h"
#include "System/PhysicsEngine.h"
#include "PhysicsInterface.h"

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
	
	appErrorf( TEXT( "Unknown collision channel %s" ), InStr.c_str() );
	return CC_Max;
}

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

	appErrorf( TEXT( "Unknown collision response %s" ), InStr.c_str() );
	return CR_Max;
}

CPhysicsEngine::CPhysicsEngine()
{}

CPhysicsEngine::~CPhysicsEngine()
{
	Shutdown();
}

void CPhysicsEngine::Init()
{
	// Init physics interface
	CPhysicsInterface::Init();

	// Init physics scene
	GPhysicsScene.Init();

	// Load default physics material
	{
		// Loading default material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			CConfigValue		configDefaultPhysMaterial = GEngineConfig.GetValue( TEXT( "Physics.Physics" ), TEXT( "DefaultPhysMaterial" ) );
			if ( configDefaultPhysMaterial.IsValid() )
			{
				std::wstring			pathAsset = configDefaultPhysMaterial.GetString();
				TAssetHandle<CAsset>	asset = GPackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
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

		// If default physics material not loaded we create in virtual package
		if ( !defaultPhysMaterial.IsAssetValid() )
		{
			PackageRef_t					package = GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CPhysicsMaterial>	physMaterial = MakeSharedPtr<CPhysicsMaterial>();

			defaultPhysMaterial		= physMaterial->GetAssetHandle();
			package->Add( defaultPhysMaterial );
		}
	}

	// Load collision profiles
	{
		CConfigValue		configCollisionProfiles = GEngineConfig.GetValue( TEXT( "Physics.Physics" ), TEXT( "CollisionProfiles" ) );
		if ( configCollisionProfiles.IsValid() )
		{
			check( configCollisionProfiles.GetType() == CConfigValue::T_Array );
			std::vector< CConfigValue >		arrayCollisionProfiles = configCollisionProfiles.GetArray();

			for ( uint32 index = 0, count = arrayCollisionProfiles.size(); index < count; ++index )
			{
				const CConfigValue&		configCollisionProfile = arrayCollisionProfiles[ index ];
				check( configCollisionProfile.GetType() == CConfigValue::T_Object );
				CConfigObject			objectCollisionProfile = configCollisionProfile.GetObject();

				SCollisionProfile		collisionProfile;
				collisionProfile.name = objectCollisionProfile.GetValue( TEXT( "Name" ) ).GetString();

				std::wstring		objectType = objectCollisionProfile.GetValue( TEXT( "ObjectType" ) ).GetString();
				collisionProfile.objectType = TextToECollisionChannel( objectType );

				std::vector< CConfigValue >		arrayCollisionResponses = objectCollisionProfile.GetValue( TEXT( "CollisionResponses" ) ).GetArray();
				for ( uint32 indexResponse = 0, countResponse = arrayCollisionResponses.size(); indexResponse < countResponse; ++indexResponse )
				{
					const CConfigValue&		configCollisionResponse = arrayCollisionResponses[ indexResponse ];
					check( configCollisionResponse.GetType() == CConfigValue::T_Object );
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
			LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Engine'collision profiles in section 'Physics.Physics:CollisionProfiles'" ) );
		}
	}
}

void CPhysicsEngine::Tick( float InDeltaTime )
{
	GPhysicsScene.Tick( InDeltaTime );
}

void CPhysicsEngine::Shutdown()
{
	// Free allocated memory
	GPhysicsScene.Shutdown();
	defaultPhysMaterial.Reset();
	CPhysicsInterface::Shutdown();
}