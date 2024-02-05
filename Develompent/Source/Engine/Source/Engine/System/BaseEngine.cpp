/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/Misc/Class.h"
#include "Core/Misc/CoreGlobals.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Containers/String.h"
#include "Core/System/Package.h"
#include "Core/System/BaseFileSystem.h"
#include "Core/System/Config.h"
#include "Core/System/BaseWindow.h"
#include "Core/System/Application.h"
#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"
#include "Engine/System/World.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/BaseEngine.h"
#include "Engine/System/InputSystem.h"
#include "Engine/Render/Shaders/BasePassShader.h"
#include "Engine/Render/Shaders/WireframeShader.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/System/CameraManager.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/UIEngine.h"

IMPLEMENT_CLASS( CBaseEngine )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CBaseEngine )

/*
==================
CBaseEngine::CBaseEngine
==================
*/
CBaseEngine::CBaseEngine()
	: bPrePass( true )
	, bAutoExposure( true )
	, exposure( 1.f )
	, exposureMin( 0.2f )
	, exposureMax( 2.f )
	, gamma( 2.2f )
{}

/*
==================
CBaseEngine::Init
==================
*/
void CBaseEngine::Init()
{
#if WITH_EDITOR
	EApplicationType	appType = CApplication::Get().GetType();
#endif // WITH_EDITOR

	// Load default texture
	{	
		// Loading default texture from packages only when we in game
#if WITH_EDITOR
		if ( appType != AT_Cooker && appType != AT_Tool )
#endif // WITH_EDITOR
		{
			CConfigValue		configDefaultTexture = g_Config.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "DefaultTexture" ) );
			if ( configDefaultTexture.IsValid() )
			{
				std::wstring			pathAsset = configDefaultTexture.GetString();
				TAssetHandle<CAsset>	asset = g_PackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
				{
					defaultTexture = asset;
				}
				else
				{
					Warnf( TEXT( "Default texture '%s' not loaded\n" ), pathAsset.c_str() );
				}
			}
			else
			{
				Warnf( TEXT( "Need set in config 'Engine' default texture in section 'Engine.Engine:DefaultTexture'\n" ) );
			}
		}

		// If default texture not loaded we create in virtual package
		if ( !defaultTexture.IsAssetValid() )
		{
			std::vector< byte >			data		= { 0, 0, 0, 0 };
			PackageRef_t				package		= g_PackageManager->LoadPackage( TEXT( "" ), true );
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
#if WITH_EDITOR
		if ( appType != AT_Cooker && appType != AT_Tool )
#endif // WITH_EDITOR
		{
			CConfigValue		configDefaultMaterial = g_Config.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "DefaultMaterial" ) );
			if ( configDefaultMaterial.IsValid() )
			{
				std::wstring			pathAsset = configDefaultMaterial.GetString();
				TAssetHandle<CAsset>	asset = g_PackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
				{
					defaultMaterial = asset;
				}
				else
				{
					Warnf( TEXT( "Default material '%s' not loaded\n" ), pathAsset.c_str() );
				}
			}
			else
			{
				Warnf( TEXT( "Need set in config 'Engine' default material in section 'Engine.Engine:DefaultMaterial'\n" ) );
			}
		}

		// If default material not loaded we create in virtual package
		if ( !defaultMaterial.IsAssetValid() )
		{
			PackageRef_t			package = g_PackageManager->LoadPackage( TEXT( "" ), true );
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
		if ( appType != AT_Cooker && appType != AT_Tool )
		{
			CConfigValue		configDefaultWireframeMaterial = g_Config.GetValue( CT_Engine, TEXT( "Editor.Editor" ), TEXT( "DefaultWireframeMaterial" ) );
			if ( configDefaultWireframeMaterial.IsValid() )
			{
				std::wstring			pathAsset	= configDefaultWireframeMaterial.GetString();
				TAssetHandle<CAsset>	asset		= g_PackageManager->FindAsset( pathAsset );
				if ( asset.IsAssetValid() )
				{
					defaultWireframeMaterial = asset;
				}
				else
				{
					Warnf( TEXT( "Default wireframe material '%s' not loaded\n" ), pathAsset.c_str() );
				}
			}
			else
			{
				Warnf( TEXT( "Need set in config 'Editor' default wireframe material in section 'Editor.Editor:DefaultWireframeMaterial'\n" ) );
			}
		}

		// If default wireframe material not loaded we create in virtual package
		if ( !defaultWireframeMaterial.IsAssetValid() )
		{
			PackageRef_t			package = g_PackageManager->LoadPackage( TEXT( "" ), true );
			TSharedPtr<CMaterial>	material = MakeSharedPtr<CMaterial>();
			material->SetAssetName( TEXT( "DefaultWireframeMaterial" ) );
			material->SetWireframe( true );

			defaultWireframeMaterial	= material->GetAssetHandle();
			package->Add( defaultWireframeMaterial );
		}
	}
#endif // WITH_EDITOR

	// Get from configs PrePass, HDRExposure, etc values
	{
		CConfigValue	configPrePass = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "PrePass" ) );
		if ( configPrePass.IsValid() )
		{
			bPrePass = configPrePass.GetBool();
		}

		CConfigValue	configExposure = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "Exposure" ) );
		if ( configExposure.IsValid() )
		{
			exposure = configExposure.GetNumber();
		}

		CConfigValue	configAutoExposure = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "AutoExposure" ) );
		if ( configAutoExposure.IsValid() )
		{
			bAutoExposure = configAutoExposure.GetBool();
		}

		CConfigValue	configExposureMin = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "ExposureMin" ) );
		if ( configExposureMin.IsValid() )
		{
			exposureMin = configExposureMin.GetNumber();
		}

		CConfigValue	configExposureMax = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "ExposureMax" ) );
		if ( configExposureMax.IsValid() )
		{
			exposureMax = configExposureMax.GetNumber();
		}

		CConfigValue	configGamma = g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "Gamma" ) );
		if ( configGamma.IsValid() )
		{
			gamma = configGamma.GetNumber();
		}
	}

	// Register default assets
	g_AssetFactory.SetDefault( defaultTexture, AT_Texture2D );
	g_AssetFactory.SetDefault( defaultMaterial, AT_Material );

	g_InputSystem->Init();
	g_UIEngine->Init();
	g_PhysicsEngine.Init();
}

/*
==================
CBaseEngine::Shutdown
==================
*/
void CBaseEngine::Shutdown()
{
	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	g_World->CleanupWorld();
	g_UIEngine->Shutdown();
	g_PhysicsEngine.Shutdown();
}

/*
==================
CBaseEngine::Tick
==================
*/
void CBaseEngine::Tick( float InDeltaSeconds )
{
	g_World->Tick( InDeltaSeconds );
	g_UIEngine->Tick( InDeltaSeconds );
	g_PhysicsEngine.Tick( InDeltaSeconds );
}

/*
==================
CBaseEngine::ProcessEvent
==================
*/
void CBaseEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == g_Window->GetID() )
		{
			g_IsRequestingExit = true;
		}
		break;
	}

	g_InputSystem->ProcessEvent( InWindowEvent );
	g_UIEngine->ProcessEvent( InWindowEvent );
	g_CameraManager->ProcessEvent( InWindowEvent );
}

/*
==================
CBaseEngine::GetMaxTickRate
==================
*/
float CBaseEngine::GetMaxTickRate() const
{
	CConfigValue		configMaxTickRate = g_Config.GetValue( CT_Engine, TEXT( "Engine.Engine" ), TEXT( "MaxTickRate" ) );
	if ( configMaxTickRate.IsValid() )
	{
		return configMaxTickRate.GetNumber();
	}

	return 0.f;
}

/*
==================
CBaseEngine::LoadMap
==================
*/
bool CBaseEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	Logf( TEXT( "Load map: %s\n" ), InMap.c_str() );

	CArchive*		archive = g_FileSystem->CreateFileReader( InMap );
	if ( !archive )
	{
		OutError = TEXT( "Map not found" );
		return false;
	}

	// Clean up and serialize world
	g_World->CleanupWorld();
	archive->SerializeHeader();
	g_World->Serialize( *archive );

	// Call garbage collector of unused packages and assets for free used memory
	g_PackageManager->GarbageCollector();
	return true;
}
