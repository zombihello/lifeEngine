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
	
	Sys_Error( TEXT( "Unknown collision channel %s" ), InStr.c_str() );
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

	Sys_Error( TEXT( "Unknown collision response %s" ), InStr.c_str() );
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
			const CJsonValue*		configDefaultPhysMaterial = CConfig::Get().GetValue( CT_Engine, TEXT( "Physics.Physics" ), TEXT( "DefaultPhysMaterial" ) );
			if ( configDefaultPhysMaterial && configDefaultPhysMaterial->IsValid() )
			{
				std::wstring			pathAsset = configDefaultPhysMaterial->GetString();
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
		const CJsonValue*		configCollisionProfiles = CConfig::Get().GetValue( CT_Engine, TEXT( "Physics.Physics" ), TEXT( "CollisionProfiles" ) );
		if ( configCollisionProfiles && configCollisionProfiles->IsValid() )
		{
			Assert( configCollisionProfiles->GetType() == JVT_Array );
			const std::vector<CJsonValue>*	arrayCollisionProfiles = configCollisionProfiles->GetArray();
			if ( arrayCollisionProfiles )
			{
				for ( uint32 index = 0, count = arrayCollisionProfiles->size(); index < count; ++index )
				{
					const CJsonValue&		configCollisionProfile = arrayCollisionProfiles->at( index );
					const CJsonObject*		objectCollisionProfile = configCollisionProfile.GetObject();
					if ( !objectCollisionProfile )
					{
						Warnf( TEXT( "Invalid 'Physics.Physics:CollisionProfiles[%i]\n" ), index );
						continue;
					}

					CollisionProfile		collisionProfile;
					const CJsonValue*		collisionProfileName = objectCollisionProfile->GetValue( TEXT( "Name" ) );
					collisionProfile.name = collisionProfileName ? collisionProfileName->GetString() : TEXT( "" );
					if ( collisionProfile.name.empty() )
					{
						Warnf( TEXT( "Invalid 'Physics.Physics:CollisionProfiles[%i]\n" ), index );
						continue;
					}

					const CJsonValue*	collisionProfileObjectType = objectCollisionProfile->GetValue( TEXT( "ObjectType" ) );
					if ( !collisionProfileObjectType )
					{
						Warnf( TEXT( "Invalid 'Physics.Physics:CollisionProfiles[%i]\n" ), index );
						continue;
					}

					std::wstring						objectType = collisionProfileObjectType->GetString();
					collisionProfile.objectType = TextToECollisionChannel( objectType );
					const CJsonValue*					collisionProfileArray = objectCollisionProfile->GetValue( TEXT( "CollisionResponses" ) );
					const std::vector<CJsonValue>*		arrayCollisionResponses = collisionProfileArray ? collisionProfileArray->GetArray() : nullptr;
					if ( arrayCollisionResponses )
					{
						for ( uint32 indexResponse = 0, countResponse = arrayCollisionResponses->size(); indexResponse < countResponse; ++indexResponse )
						{
							const CJsonValue&		configCollisionResponse = arrayCollisionResponses->at( indexResponse );
							const CJsonObject*		objectCollisionResponse = configCollisionResponse.GetObject();
							if ( !objectCollisionResponse )
							{
								Warnf( TEXT( "Invalid collision response %i at 'Physics.Physics:CollisionProfiles[%i]\n" ), indexResponse, index );
								continue;
							}

							const CJsonValue*		responseName = objectCollisionResponse->GetValue( TEXT( "Name" ) );
							const CJsonValue*		responseValue = objectCollisionResponse->GetValue( TEXT( "Value" ) );
							std::wstring			name = responseName ? responseName->GetString() : TEXT( "" );
							std::wstring			value = responseValue ? responseValue->GetString() : TEXT( "" );
							if ( name.empty() || value.empty() )
							{
								Warnf( TEXT( "Invalid collision response %i at 'Physics.Physics:CollisionProfiles[%i]\n" ), indexResponse, index );
								continue;
							}
							collisionProfile.responses[TextToECollisionChannel( name )] = TextToECollisionResponse( value );
						}
					}

					collisionProfiles.insert( std::make_pair( collisionProfile.name, collisionProfile ) );
				}
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