#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "EngineDefines.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectGlobals.h"
#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "System/World.h"
#include "Render/Viewport.h"
#include "Render/RenderingThread.h"
#include "System/EditorEngine.h"
#include "System/ActorFactory.h"
#include "System/Package.h"
#include "System/SplashScreen.h"
#include "System/Archive.h"
#include "System/InputSystem.h"
#include "System/Config.h"
#include "System/BaseWindow.h"
#include "System/AssetsImport.h"
#include "Actors/Actor.h"
#include "UIEngine.h"

// Actors
#include "Actors/PlayerStart.h"
#include "Actors/StaticMesh.h"
#include "Actors/Audio.h"

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

/**
 * @ingroup WorldEd
 * @brief Viewport client for render editor interface
 */
class CEditorInterfaceViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override
	{
		g_UIEngine->BeginDraw();
		g_UIEngine->EndDraw();
	}
};

/*
==================
CEditorEngine::CEditorEngine
==================
*/
CEditorEngine::CEditorEngine()
	: currentEditorMode( EM_Default )
	, editorInterfaceViewportClient( nullptr )
{}

/*
==================
CEditorEngine::Init
==================
*/
void CEditorEngine::Init()
{
	g_EditorEngine = this;

	// If failed serialize TOC file, we generate new TOC file and serialize data to archive
	if ( !SerializeTOC() )
	{
		Sys_SetSplashText( STT_StartupProgress, TEXT( "Prepare engine content" ) );
		AddTOCEntries( GetEngineContentDir() );

		Sys_SetSplashText( STT_StartupProgress, TEXT( "Prepare game content" ) );
		AddTOCEntries( GetGameContentDir() );

		SerializeTOC( true );
	}

	Super::Init();

	// Register actor factory for assets
	g_ActorFactory.Register( AT_StaticMesh,	&AStaticMesh::SpawnActorAsset );
	g_ActorFactory.Register( AT_AudioBank,	&AAudio::SpawnActorAsset );

	// Register importers of assets
	g_AssetFactory.RegisterImporter( &CTexture2DImporter::Import, &CTexture2DImporter::Reimport, nullptr, CTexture2DImporter::GetSupportedExtensions(), AT_Texture2D );
	g_AssetFactory.RegisterImporter( &CAudioBankImporter::Import, &CAudioBankImporter::Reimport, nullptr, CAudioBankImporter::GetSupportedExtensions(), AT_AudioBank );
	g_AssetFactory.RegisterImporter( &CStaticMeshImporter::Import, &CStaticMeshImporter::Reimport, &CStaticMeshImporter::ShowImportSettings, CStaticMeshImporter::GetSupportedExtensions(), AT_StaticMesh );

	// Create window and main viewport
	uint32						windowWidth		= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight	= g_Config.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();

	g_Window->SetTitle( GetEditorName().c_str() );
	g_Window->SetSize( windowWidth, windowHeight );

	editorInterfaceViewportClient	= new CEditorInterfaceViewportClient();
	CViewport*	viewport			= new CViewport();
	viewport->SetViewportClient( editorInterfaceViewportClient );
	viewport->Update( false, windowWidth, windowHeight, g_Window->GetHandle() );
	viewports.push_back( viewport );

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

	// Init all windows
	editorWindow						= MakeSharedPtr<CEditorWindow>();
	editorWindow->Init();

	actorClassesWindow					= MakeSharedPtr<CActorClassesWindow>( CString::Format( TEXT( "%s Classes" ), ANSI_TO_TCHAR( IMGUI_ICON_CODE ) ), AActor::StaticClass() );
	actorClassesWindow->Init();
	
	actorPropertiesWindow				= MakeSharedPtr<CActorPropertiesWindow>( CString::Format( TEXT( "%s Properties" ), ANSI_TO_TCHAR( IMGUI_ICON_MENU ) ) );
	actorPropertiesWindow->Init();

	contentBrowserWindow				= MakeSharedPtr<CContentBrowserWindow>( CString::Format( TEXT( "%s Content" ), ANSI_TO_TCHAR( IMGUI_ICON_DATABASE ) ) );
	contentBrowserWindow->Init();

	explorerLevelWindow					= MakeSharedPtr<CExplorerLevelWindow>( CString::Format( TEXT( "%s Explorer Level" ), ANSI_TO_TCHAR( IMGUI_ICON_GLOBE ) ) );
	explorerLevelWindow->Init();

	logsWindow							= MakeSharedPtr<CLogsWindow>( CString::Format( TEXT( "%s Logs" ), ANSI_TO_TCHAR( IMGUI_ICON_FILETEXT ) ) );
	logsWindow->Init();

	viewportWindows[LVT_OrthoXY]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho XY" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoXY );
	viewportWindows[LVT_OrthoXY]->Init();

	viewportWindows[LVT_OrthoXZ]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho XZ" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoXZ );
	viewportWindows[LVT_OrthoXZ]->Init();

	viewportWindows[LVT_OrthoYZ]		= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Ortho YZ" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), false, LVT_OrthoYZ );
	viewportWindows[LVT_OrthoYZ]->Init();

	viewportWindows[LVT_Perspective]	= MakeSharedPtr<CLevelViewportWindow>( CString::Format( TEXT( "%s Perspective" ), ANSI_TO_TCHAR( IMGUI_ICON_CAMERA ) ), true, LVT_Perspective );
	viewportWindows[LVT_Perspective]->Init();
}

/*
==================
CEditorEngine::Tick
==================
*/
void CEditorEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );

	// Update viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[index]->Tick( InDeltaSeconds );
	}
	
	// Reset input events after engine tick
	g_InputSystem->ResetEvents();

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	for ( int32 index = viewports.size()-1; index >= 0; --index )
	{
		viewports[index]->Draw();
	}
}

/*
==================
CEditorEngine::Shutdown
==================
*/
void CEditorEngine::Shutdown()
{
	Super::Shutdown();

	// Destroy viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[index]->Update( true, 0, 0, ( void* )nullptr );
	}
	FlushRenderingCommands();
	viewports.clear();

	if ( editorInterfaceViewportClient )
	{
		delete editorInterfaceViewportClient;
		editorInterfaceViewportClient = nullptr;
	}
	g_EditorEngine = nullptr;
}

/*
==================
CEditorEngine::ProcessEvent
==================
*/
void CEditorEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case WindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == g_Window->GetID() )
		{
			g_IsRequestingExit = true;
		}
		break;

	case WindowEvent::T_WindowResize:
		if ( InWindowEvent.events.windowResize.windowId == g_Window->GetID() )
		{
			viewports[0]->Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, g_Window->GetHandle() );
		}
		break;
	}
}

/*
==================
CEditorEngine::PrintLogToWidget
==================
*/
void CEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{
	if ( logsWindow && logsWindow->IsInit() )
	{
		logsWindow->PrintLog( InLogType, InMessage );
	}
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
	if ( g_World )
	{
		g_World->RemoveFromRoot();
		g_World = nullptr;
	}

	CObjectPackage*		mapPackage = CObjectPackage::CreatePackage( nullptr, TEXT( "Unknown" ) );
	g_World				= new( mapPackage, TEXT( "TheWorld" ), OBJECT_Public ) CWorld();
	g_World->AddToRoot();
	CObjectGC::Get().CollectGarbage( GARBAGE_COLLECTION_KEEPFLAGS );
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
	if ( g_World )
	{
		g_World->UnselectAllActors();
	}

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
	Assert( g_World );
	Logf( TEXT( "Save map: %s\n" ), InMap.c_str() );

	g_World->GetOutermost()->SetName( CFilename( InMap ).GetBaseFilename().c_str() );
	if ( !CObjectPackage::SavePackage( g_World->GetOutermost(), g_World, 0, InMap.c_str(), SAVE_None ) )
	{
		OutError = TEXT( "Failed to save map" );
		return false;
	}

	EditorDelegates::onEditorSavedMap.Broadcast();
	return true;
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
CEditorEngine::GetEditorName
==================
*/
std::wstring CEditorEngine::GetEditorName() const
{
#if PLATFORM_WINDOWS
#if _WIN64
	const std::wstring				platformBitsString( TEXT( "64" ) );
#else
	const std::wstring				platformBitsString( TEXT( "32" ) );
#endif // _WIN64
#else
#error Insert court bitness of your platform
#endif // PLATFORM_WINDOWS
	
	return CString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), g_GameName.c_str(), platformBitsString.c_str(), g_RHI->GetRHIName() );
}