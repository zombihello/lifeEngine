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
#include "Render/RenderingThread.h"
#include "System/CameraManager.h"

IMPLEMENT_CLASS( LBaseEngine )

void LBaseEngine::Init()
{
	// Load default texture
	{	
		// Loading default texture from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			FConfigValue		configDefaultTexture = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "DefaultTexture" ) );
			if ( configDefaultTexture.IsValid() )
			{
				std::wstring		pathAsset = configDefaultTexture.GetString();
				TWeakPtr<FAsset>	asset = GPackageManager->FindAsset( pathAsset );
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

		// If default texture not loaded we create in virtual package
		if ( !defaultTexture.Pin() )
		{
			std::vector< byte >		data		= { 0, 0, 0, 0 };
			FPackageRef				package		= GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<FTexture2D>	texture2D	= MakeSharedPtr<FTexture2D>();
			texture2D->SetData( PF_A8R8G8B8, 1, 1, data );
			
			package->Add( texture2D );
			defaultTexture = texture2D;
		}
	}

	// Load default material
	{
		// Loading default material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			FConfigValue		configDefaultMaterial = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "DefaultMaterial" ) );
			if ( configDefaultMaterial.IsValid() )
			{
				std::wstring		pathAsset = configDefaultMaterial.GetString();
				TWeakPtr<FAsset>	asset = GPackageManager->FindAsset( pathAsset );
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

		// If default material not loaded we create in virtual package
		if ( !defaultMaterial.Pin() )
		{
			FPackageRef				package = GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<FMaterial>	material = MakeSharedPtr<FMaterial>();

			package->Add( material );
			defaultMaterial = material;
		}
	}

	// Load default wireframe material (only for build with editor)
#if WITH_EDITOR
	{
		// Loading default wireframe material from packages only when we in game
		if ( !GIsCooker && !GIsCommandlet )
		{
			FConfigValue		configDefaultWireframeMaterial = GEditorConfig.GetValue( TEXT( "Editor.Editor" ), TEXT( "DefaultWireframeMaterial" ) );
			if ( configDefaultWireframeMaterial.IsValid() )
			{
				std::wstring		pathAsset	= configDefaultWireframeMaterial.GetString();
				TWeakPtr<FAsset>	asset		= GPackageManager->FindAsset( pathAsset );
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

		// If default wireframe material not loaded we create in virtual package
		if ( !defaultWireframeMaterial.Pin() )
		{
			FPackageRef				package = GPackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<FMaterial>	material = MakeSharedPtr<FMaterial>();
			material->SetWireframe( true );

			package->Add( material );
			defaultWireframeMaterial = material;
		}
	}
#endif // WITH_EDITOR

	GInputSystem->Init();
	GUIEngine->Init();
	GPhysicsEngine.Init();
}

void LBaseEngine::Shutdown()
{
	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

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

	// Serialize world
	archive->SerializeHeader();
	GWorld->Serialize( *archive );
	
	// Call garbage collector of unused packages and assets
	GPackageManager->GarbageCollector();

	// Begin play of game
	GWorld->BeginPlay();
	return true;
}
