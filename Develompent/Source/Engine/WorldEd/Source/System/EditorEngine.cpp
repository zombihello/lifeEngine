#include "Containers/String.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "EngineDefines.h"
#include "Misc/Class.h"
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
CEditorEngine::~CEditorEngine
==================
*/
CEditorEngine::~CEditorEngine()
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

	// Init all windows
	editorWindow						= MakeSharedPtr<CEditorWindow>();
	editorWindow->Init();

	actorClassesWindow					= MakeSharedPtr<CActorClassesWindow>( TEXT( "Classes" ), AActor::StaticClass() );
	actorClassesWindow->Init();
	
	actorPropertiesWindow				= MakeSharedPtr<CActorPropertiesWindow>( TEXT( "Properties" ) );
	actorPropertiesWindow->Init();

	contentBrowserWindow				= MakeSharedPtr<CContentBrowserWindow>( TEXT( "Content" ) );
	contentBrowserWindow->Init();

	explorerLevelWindow					= MakeSharedPtr<CExplorerLevelWindow>( TEXT( "Explorer Level" ) );
	explorerLevelWindow->Init();

	logsWindow							= MakeSharedPtr<CLogsWindow>( TEXT( "Logs" ) );
	logsWindow->Init();

	viewportWindows[LVT_OrthoXY]		= MakeSharedPtr<CLevelViewportWindow>( TEXT( "Ortho XY" ), ENGINE_2D ? true : false, LVT_OrthoXY );
	viewportWindows[LVT_OrthoXY]->Init();

	viewportWindows[LVT_OrthoXZ]		= MakeSharedPtr<CLevelViewportWindow>( TEXT( "Ortho XZ" ), false, LVT_OrthoXZ );
	viewportWindows[LVT_OrthoXZ]->Init();

	viewportWindows[LVT_OrthoYZ]		= MakeSharedPtr<CLevelViewportWindow>( TEXT( "Ortho YZ" ), false, LVT_OrthoYZ );
	viewportWindows[LVT_OrthoYZ]->Init();

	viewportWindows[LVT_Perspective]	= MakeSharedPtr<CLevelViewportWindow>( TEXT( "Perspective" ), ENGINE_2D ? false : true, LVT_Perspective );
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
void CEditorEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == g_Window->GetID() )
		{
			g_IsRequestingExit = true;
		}
		break;

	case SWindowEvent::T_WindowResize:
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
	g_World->CleanupWorld();
	g_PackageManager->GarbageCollector();
	
	SEditorDelegates::onEditorCreatedNewMap.Broadcast();
}

/*
==================
CEditorEngine::LoadMap
==================
*/
bool CEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	if ( !Super::LoadMap( InMap, OutError ) )
	{
		Warnf( TEXT( "Failed loading map '%s'. Error: %s\n" ), InMap.c_str(), OutError.c_str() );
		NewMap();
	}
	else
	{
		SEditorDelegates::onEditorLoadedMap.Broadcast();
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

	SEditorDelegates::onEditorSavedMap.Broadcast();
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