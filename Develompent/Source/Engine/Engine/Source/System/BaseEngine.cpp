#include "Misc/Class.h"
#include "Misc/PhysicsGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Containers/String.h"
#include "System/Config.h"
#include "System/World.h"
#include "System/Package.h"
#include "System/BaseFileSystem.h"
#include "System/PhysicsEngine.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "System/InputSystem.h"
#include "Misc/UIGlobals.h"
#include "UIEngine.h"
#include "Render/Shaders/BasePassShader.h"
#include "Render/Shaders/WireframeShader.h"
#include "System/CameraManager.h"

IMPLEMENT_CLASS( LBaseEngine )

void LBaseEngine::Init()
{
	// Load default texture
	{
		std::vector< byte >		data = { 0, 0, 0, 0 };
		defaultTexture = new FTexture2D();						// } Default texture without packages
		defaultTexture->SetData( PF_A8R8G8B8, 1, 1, data );
		
		// Loading default texture from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
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
	}

	// Load default material
	{
		defaultMaterial = new FMaterial();
		defaultMaterial->SetShader( FBasePassVertexShader::staticType );
		defaultMaterial->SetShader( FBasePassPixelShader::staticType );

		// Loading default material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
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
				LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Engine' default material in section 'Engine.Engine:DefaultMaterial'" ) );
			}
		}
	}

	// Load default wireframe material (only for build with editor)
#if WITH_EDITOR
	{
		defaultWireframeMaterial = new FMaterial();
		defaultWireframeMaterial->SetShader( FWireframeVertexShader::staticType );
		defaultWireframeMaterial->SetShader( FWireframePixelShader::staticType );
		defaultWireframeMaterial->SetWireframe( true );

		// Loading default wireframe material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			FConfigValue		configDefaultWireframeMaterial = GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "DefaultWireframeMaterial" ) );
			if ( configDefaultWireframeMaterial.IsValid() )
			{
				std::wstring	pathAsset	= configDefaultWireframeMaterial.GetString();
				FAssetRef		asset		= GPackageManager->FindAsset( pathAsset );
				if ( asset )
				{
					defaultWireframeMaterial = asset;
				}
				else
				{
					LE_LOG( LT_Warning, LC_Init, TEXT( "Default wireframe material '%s' not loaded" ), pathAsset.c_str() );
				}
			}
			else
			{
				LE_LOG( LT_Warning, LC_Init, TEXT( "Need set in config 'Editor' default wireframe material in section 'Editor.Editor:DefaultWireframeMaterial'" ) );
			}
		}
	}
#endif // WITH_EDITOR

	GInputSystem->Init();
	GUIEngine->Init();
	GPhysicsEngine.Init();
}

void LBaseEngine::Shutdown()
{
	GWorld->CleanupWorld();
	GUIEngine->Shutdown();
	GPhysicsEngine.Shutdown();
}

void LBaseEngine::Tick( float InDeltaSeconds )
{
	GPhysicsEngine.Tick( InDeltaSeconds );
}

void LBaseEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	GInputSystem->ProcessEvent( InWindowEvent );
	GUIEngine->ProcessEvent( InWindowEvent );
	GCameraManager->ProcessEvent( InWindowEvent );
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

bool LBaseEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	LE_LOG( LT_Log, LC_General, TEXT( "Load map: %s" ), InMap.c_str() );

	FArchive*		archive = GFileSystem->CreateFileReader( FString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s" ), GCookedDir.c_str(), InMap.c_str() ) );
	if ( !archive )
	{
		OutError = TEXT( "Map not found" );
		return false;
	}

	archive->SerializeHeader();
	GWorld->Serialize( *archive );
	GWorld->BeginPlay();
	return true;
}
