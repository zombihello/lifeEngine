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

#include "Core/Misc/CoreGlobals.h"
#include "Core/Misc/Class.h"
#include "Core/System/BaseFileSystem.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/SplashScreen.h"
#include "Engine/System/World.h"
#include "Editor/Misc/EditorGlobals.h"
#include "Editor/System/EditorEngine.h"
#include "Editor/System/ActorFactory.h"
#include "Editor/System/AssetsImport.h"

// Actors
#include "Engine/Actors/PlayerStart.h"
#include "Engine/Actors/StaticMesh.h"
#include "Engine/Actors/Audio.h"

IMPLEMENT_CLASS( CEditorEngine )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CEditorEngine )

/** Table of icons */
static const tchar*	s_IconPaths[] =
{
	TEXT( "Icons/Color_R.png" ),			// IT_Color_R
	TEXT( "Icons/Color_G.png" ),			// IT_Color_G
	TEXT( "Icons/Color_B.png" ),			// IT_Color_B
	TEXT( "Icons/Color_A.png" ),			// IT_Color_A
	TEXT( "Icons/Import.png" ),				// IT_Import
	TEXT( "Icons/CSW_PasteAsset.png" ),		// IT_Insert
	TEXT( "Icons/CSW_ShowAsset.png" ),		// IT_Browse
	TEXT( "Icons/CSW_RemoveAsset.png" ),	// IT_Remove
	TEXT( "Icons/Properties_Add.png" ),		// IT_Add
	TEXT( "Icons/Tool_Select.png" ),		// IT_ToolSelect
	TEXT( "Icons/Tool_Translate.png" ),		// IT_ToolTranslate
	TEXT( "Icons/Tool_Rotate.png" ),		// IT_ToolRotate
	TEXT( "Icons/Tool_Scale.png" ),			// IT_ToolScale
	TEXT( "Icons/PlayStandaloneGame.png" ),	// IT_PlayStandaloneGame
	TEXT( "Thumbnails/Unknown.png" ),		// IT_Thumbnail_Asset_Unknown
	TEXT( "Thumbnails/Texture.png" ),		// IT_Thumbnail_Asset_Texture2D
	TEXT( "Thumbnails/Material.png" ),		// IT_Thumbnail_Asset_Material
	TEXT( "Thumbnails/Script.png" ),		// IT_Thumbnail_Asset_Script
	TEXT( "Thumbnails/StaticMesh.png" ),	// IT_Thumbnail_Asset_StaticMesh
	TEXT( "Thumbnails/Audio.png" ),			// IT_Thumbnail_Asset_AudioBank
	TEXT( "Thumbnails/PhysMaterial.png" )	// IT_Thumbnail_Asset_PhysicsMaterial
};
static_assert( ARRAY_COUNT( s_IconPaths ) == IT_Num, "Need full init s_IconPaths array" );

/*
==================
CEditorEngine::CEditorEngine
==================
*/
CEditorEngine::CEditorEngine()
	: selectionColor( 32, 0, 128, 255 )
	, currentEditorMode( EM_Default )
{
	g_EditorEngine = this;
}

/*
==================
CEditorEngine::~CEditorEngine
==================
*/
CEditorEngine::~CEditorEngine()
{
	g_EditorEngine = false;
}

/*
==================
CEditorEngine::Init
==================
*/
void CEditorEngine::Init()
{
	Super::Init();

	// If failed serialize TOC file, we generate new TOC file and serialize data to archive
	if ( !SerializeTOC() )
	{
		Sys_SetSplashText( STT_StartupProgress, TEXT( "Prepare engine content" ) );
		AddTOCEntries( GetEngineContentDir() );

		Sys_SetSplashText( STT_StartupProgress, TEXT( "Prepare game content" ) );
		AddTOCEntries( GetGameContentDir() );

		SerializeTOC( true );
	}

	// Register actor factory for assets
	g_ActorFactory.Register( AT_StaticMesh,	&AStaticMesh::SpawnActorAsset );
	g_ActorFactory.Register( AT_AudioBank,	&AAudio::SpawnActorAsset );

	// Register importers of assets
	g_AssetFactory.RegisterImporter( &CTexture2DImporter::Import, &CTexture2DImporter::Reimport, nullptr, CTexture2DImporter::GetSupportedExtensions(), AT_Texture2D );
	g_AssetFactory.RegisterImporter( &CAudioBankImporter::Import, &CAudioBankImporter::Reimport, nullptr, CAudioBankImporter::GetSupportedExtensions(), AT_AudioBank );
	g_AssetFactory.RegisterImporter( &CStaticMeshImporter::Import, &CStaticMeshImporter::Reimport, &CStaticMeshImporter::ShowImportSettings, CStaticMeshImporter::GetSupportedExtensions(), AT_StaticMesh );

	// Load icons
	{
		std::wstring	errorMsg;
		PackageRef_t	package = g_PackageManager->LoadPackage( TEXT( "" ), true );
		Assert( package );

		for ( uint32 index = 0; index < IT_Num; ++index )
		{
			const std::wstring				assetName = CString::Format( TEXT( "EditorIcon_%X" ), index );
			TAssetHandle<CTexture2D>&		assetHandle = icons[index];
			assetHandle = package->Find( assetName );
			if ( !assetHandle.IsAssetValid() )
			{
				std::vector<TSharedPtr<CAsset>>		result;
				if ( !CTexture2DImporter::Import( Sys_BaseDir() + TEXT( "Engine/Editor/" ) + s_IconPaths[index], result, errorMsg ) )
				{
					Warnf( TEXT( "Fail to load editor icon '%s' for type 0x%X. Message: %s\n" ), s_IconPaths[index], index, errorMsg.c_str() );
					assetHandle = g_Engine->GetDefaultTexture();
				}
				else
				{
					TSharedPtr<CTexture2D>		texture2D = result[0];
					texture2D->SetAssetName( assetName );
					assetHandle = texture2D->GetAssetHandle();
					package->Add( assetHandle );
				}
			}
		}
	}
}

/*
==================
CEditorEngine::PrintLogToWidget
==================
*/
void CEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{}

/*
==================
CEditorEngine::GetSelectedAsset
==================
*/
std::wstring CEditorEngine::GetSelectedAsset() const
{
	return TEXT( "" );
}

/*
==================
CEditorEngine::SetSelectedAsset
==================
*/
void CEditorEngine::SetSelectedAsset( const std::wstring& InAssetReference )
{}

/*
==================
CEditorEngine::GetEditorName
==================
*/
std::wstring CEditorEngine::GetEditorName() const
{
	return TEXT( "Editor" );
}

/*
==================
CEditorEngine::NewMap
==================
*/
void CEditorEngine::NewMap()
{
	Logf( TEXT( "Create a new map\n" ) );
	
	// Clean up world and call garbage collector of unused packages and assets
	g_World->CleanupWorld();
	g_PackageManager->GarbageCollector();
	
	EditorDelegates::onEditorCreatedNewMap.Broadcast();
}

/*
==================
CEditorEngine::LoadMap
==================
*/
bool CEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	g_World->UnselectAllActors();
	if ( !Super::LoadMap( InMap, OutError ) )
	{
		Warnf( TEXT( "Failed loading map '%s'. Error: %s\n" ), InMap.c_str(), OutError.c_str() );
		NewMap();
	}
	else
	{
		EditorDelegates::onEditorLoadedMap.Broadcast();
	}

	return true;
}

/*
==================
CEditorEngine::SaveMap
==================
*/
bool CEditorEngine::SaveMap( const std::wstring& InMap, std::wstring& OutError )
{
	Logf( TEXT( "Save map: %s\n" ), InMap.c_str() );

	CArchive*	arWorld = g_FileSystem->CreateFileWriter( InMap );
	if ( !arWorld )
	{
		OutError = TEXT( "Failed open archive" );
		return false;
	}

	arWorld->SetType( AT_World );
	arWorld->SerializeHeader();
	g_World->Serialize( *arWorld );
	delete arWorld;

	EditorDelegates::onEditorSavedMap.Broadcast();
	return true;
}

/*
==================
CEditorEngine::SerializeTOC
==================
*/
bool CEditorEngine::SerializeTOC( bool InIsSave /* = false */ )
{
	std::wstring	pathTOC = Sys_GameDir() + ( PATH_SEPARATOR TEXT( "Content" ) ) + PATH_SEPARATOR + g_TableOfContents.GetNameTOC();
	CArchive* archiveTOC = !InIsSave ? g_FileSystem->CreateFileReader( pathTOC ) : g_FileSystem->CreateFileWriter( pathTOC );
	if ( archiveTOC )
	{
		g_TableOfContents.Serialize( *archiveTOC );
		delete archiveTOC;
		return true;
	}
	else
	{
		Warnf( TEXT( "Failed serialize TOC file '%s'\n" ), pathTOC.c_str() );
		return false;
	}
}

/*
==================
CEditorEngine::AddTOCEntries
==================
*/
void CEditorEngine::AddTOCEntries( const std::wstring& InRootDir )
{
	std::vector< std::wstring >		files = g_FileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring		file = files[ index ];
		std::size_t			dotPos = file.find_last_of( TEXT( "." ) );
		std::wstring		fullPath = InRootDir + TEXT( "/" ) + file;

		if ( dotPos == std::wstring::npos )
		{
			AddTOCEntries( fullPath );
			continue;
		}

		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "pak" ) )
		{
			g_TableOfContents.AddEntry( fullPath );
		}
	}
}