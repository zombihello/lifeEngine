#include "Misc/Class.h"
#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "System/Config.h"
#include "System/World.h"
#include "System/Package.h"
#include "System/BaseFileSystem.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "System/InputSystem.h"
#include "Misc/UIGlobals.h"
#include "UIEngine.h"
#include "Render/Shaders/BasePassShader.h"

IMPLEMENT_CLASS( LBaseEngine )

void LBaseEngine::Init()
{
	// Load default texture
	{
		std::vector< byte >		data = { 0, 0, 0, 0 };
		defaultTexture = new FTexture2D();						// } Default texture without packages
		defaultTexture->SetData( PF_A8R8G8B8, 1, 1, data );
		
		FConfigValue		configDefaultTexture = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "DefaultTexture" ) );
		if ( configDefaultTexture.IsValid() )
		{
			std::wstring	pathAsset = configDefaultTexture.GetString();
			FAssetRef		asset = GPackageManager->FindAsset( pathAsset );
			if ( asset )
			{
				defaultTexture = asset;
			}
			else
			{
				LE_LOG( LT_Warning, LC_Init, TEXT( "Default texture '%s' not loaded" ), pathAsset.c_str() );
			}
		}
		else
		{
			LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Engine' default texture in section 'Engine.Engine:DefaultTexture'" ) );
		}
	}

	// Load default material
	{
		defaultMaterial = new FMaterial();
		defaultMaterial->SetShader( FBasePassVertexShader::staticType );
		defaultMaterial->SetShader( FBasePassPixelShader::staticType );

		FConfigValue		configDefaultMaterial = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "DefaultMaterial" ) );
		if ( configDefaultMaterial.IsValid() )
		{
			std::wstring	pathAsset = configDefaultMaterial.GetString();
			FAssetRef		asset = GPackageManager->FindAsset( pathAsset );
			if ( asset )
			{
				defaultMaterial = asset;
			}
			else
			{
				LE_LOG( LT_Warning, LC_Init, TEXT( "Default material '%s' not loaded" ), pathAsset.c_str() );
			}
		}
		else
		{
			LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Engine' default texture in section 'Engine.Engine:DefaultMaterial'" ) );
		}
	}

	GInputSystem->Init();
	GUIEngine->Init();
}

void LBaseEngine::Shutdown()
{
	GUIEngine->Shutdown();
}

void LBaseEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	GInputSystem->ProcessEvent( InWindowEvent );
	GUIEngine->ProcessEvent( InWindowEvent );
}

float LBaseEngine::GetMaxTickRate() const
{
	FConfigValue		configMaxTickRate = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "MaxTickRate" ) );
	if ( configMaxTickRate.IsValid() )
	{
		return configMaxTickRate.GetNumber();
	}

	return 0.f;
}
#include "Actors/StaticMesh.h"
#include "Actors/Sprite.h"
bool LBaseEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	LE_LOG( LT_Log, LC_General, TEXT( "Load map: %s" ), InMap.c_str() );

	FArchive* archive = GFileSystem->CreateFileReader( appBaseDir() + FString::Format( TEXT( "Content/Maps/%s" ), InMap.c_str() ) );
	if ( !archive )
	{
		OutError = TEXT( "Map not found" );
		return false;
	}

	archive->SerializeHeader();
	GWorld->Serialize( *archive );
	GWorld->SpawnActor< AStaticMesh >( FVector( 0.f, 0.f, 0.f ) );
    GWorld->SpawnActor< ASprite >( FVector( 0.f, -150.f, 0.f ) );
	GWorld->BeginPlay();
	return true;
}
