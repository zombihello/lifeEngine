#include "Misc/Class.h"
#include "Misc/CoreGlobals.h"
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
#include "Render/RenderingThread.h"
#include "System/CameraManager.h"

IMPLEMENT_CLASS( CBaseEngine )

void CBaseEngine::Init()
{
	// Load default texture
	{	
		// Loading default texture from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			CConfigValue		configDefaultTexture = GConfig.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "DefaultTexture" ) );
			if ( configDefaultTexture.IsValid() )
			{
				std::wstring			pathAsset = configDefaultTexture.GetString();
				TAssetHandle<CAsset>	asset = GPackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
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

		// If default texture not loaded we create in virtual package
		if ( !defaultTexture.IsAssetValid() )
		{
			std::vector< byte >			data		= { 0, 0, 0, 0 };
			PackageRef_t				package		= GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CTexture2D>		texture2D	= MakeSharedPtr<CTexture2D>();
			texture2D->SetAssetName( TEXT( "DefaultTexture" ) );
			texture2D->SetData( PF_A8R8G8B8, 1, 1, data );

			defaultTexture		= texture2D->GetAssetHandle();
			package->Add( defaultTexture );
		}
	}

	// Load default material
	{
		// Loading default material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			CConfigValue		configDefaultMaterial = GConfig.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "DefaultMaterial" ) );
			if ( configDefaultMaterial.IsValid() )
			{
				std::wstring			pathAsset = configDefaultMaterial.GetString();
				TAssetHandle<CAsset>	asset = GPackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
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

		// If default material not loaded we create in virtual package
		if ( !defaultMaterial.IsAssetValid() )
		{
			PackageRef_t			package = GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CMaterial>	material = MakeSharedPtr<CMaterial>();
			material->SetAssetName( TEXT( "DefaultMaterial" ) );

			defaultMaterial		= material->GetAssetHandle();
			package->Add( defaultMaterial );
		}
	}

	// Load default wireframe material (only for build with editor)
#if WITH_EDITOR
	{
		// Loading default wireframe material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			CConfigValue		configDefaultWireframeMaterial = GConfig.GetValue( CT_Engine, TEXT( "Editor.Editor" ), TEXT( "DefaultWireframeMaterial" ) );
			if ( configDefaultWireframeMaterial.IsValid() )
			{
				std::wstring			pathAsset	= configDefaultWireframeMaterial.GetString();
				TAssetHandle<CAsset>	asset		= GPackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
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

		// If default wireframe material not loaded we create in virtual package
		if ( !defaultWireframeMaterial.IsAssetValid() )
		{
			PackageRef_t			package = GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CMaterial>	material = MakeSharedPtr<CMaterial>();
			material->SetAssetName( TEXT( "DefaultWireframeMaterial" ) );
			material->SetWireframe( true );

			defaultWireframeMaterial	= material->GetAssetHandle();
			package->Add( defaultWireframeMaterial );
		}
	}
#endif // WITH_EDITOR

	// Register default assets
	GAssetFactory.SetDefault( defaultTexture, AT_Texture2D );
	GAssetFactory.SetDefault( defaultMaterial, AT_Material );

	GInputSystem->Init();
	GUIEngine->Init();
	GPhysicsEngine.Init();
}

void CBaseEngine::Shutdown()
{
	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	GWorld->CleanupWorld();
	GUIEngine->Shutdown();
	GPhysicsEngine.Shutdown();
}

void CBaseEngine::Tick( float InDeltaSeconds )
{
	GUIEngine->Tick( InDeltaSeconds );
	GPhysicsEngine.Tick( InDeltaSeconds );
}

void CBaseEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	GInputSystem->ProcessEvent( InWindowEvent );
	GUIEngine->ProcessEvent( InWindowEvent );
	GCameraManager->ProcessEvent( InWindowEvent );
}

float CBaseEngine::GetMaxTickRate() const
{
	CConfigValue		configMaxTickRate = GConfig.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "MaxTickRate" ) );
	if ( configMaxTickRate.IsValid() )
	{
		return configMaxTickRate.GetNumber();
	}

	return 0.f;
}

bool CBaseEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	LE_LOG( LT_Log, LC_General, TEXT( "Load map: %s" ), InMap.c_str() );

	CArchive*		archive = GFileSystem->CreateFileReader( CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s" ), GCookedDir.c_str(), InMap.c_str() ) );
	if ( !archive )
	{
		OutError = TEXT( "Map not found" );
		return false;
	}

	// Serialize world
	archive->SerializeHeader();
	GWorld->Serialize( *archive );
	
	// Call garbage collector of unused packages and assets
	GPackageManager->GarbageCollector();
	return true;
}
