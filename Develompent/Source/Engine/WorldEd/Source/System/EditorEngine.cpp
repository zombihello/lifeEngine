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
#include "System/World.h"
#include "Render/Viewport.h"
#include "Render/RenderingThread.h"
#include "Render/EditorInterfaceViewportClient.h"
#include "System/EditorEngine.h"
#include "System/ActorFactory.h"
#include "System/Package.h"
#include "System/SplashScreen.h"
#include "System/Archive.h"
#include "System/InputSystem.h"
#include "System/Config.h"
#include "System/BaseWindow.h"
#include "System/AssetsImport.h"

// Actors
#include "Actors/PlayerStart.h"
#include "Actors/StaticMesh.h"
#include "Actors/Audio.h"

IMPLEMENT_CLASS( CEditorEngine )

CEditorEngine::CEditorEngine()
	: currentEditorMode( EM_Default )
	, editorInterfaceViewportClient( nullptr )
	, actorClassesWindow( TEXT( "Classes" ) )
	, actorPropertiesWindow( TEXT( "Properties" ) )
	, contentBrowserWindow( TEXT( "Content" ) )
	, explorerLevelWindow( TEXT( "Explorer Level" ) )
	, logsWindow( TEXT( "Logs" ) )
	, viewportWindows{ 
		CViewportWindow( TEXT( "Ortho XY" ), false, LVT_OrthoXY ),
		CViewportWindow( TEXT( "Ortho XZ" ), false, LVT_OrthoXZ ),
		CViewportWindow( TEXT( "Ortho YZ" ), false, LVT_OrthoYZ ),
		CViewportWindow( TEXT( "Perspective" ), true, LVT_Perspective )
	}
{}

CEditorEngine::~CEditorEngine()
{}

void CEditorEngine::Init()
{
	GEditorEngine = this;

	// If failed serialize TOC file, we generate new TOC file and serialize data to archive
	if ( !SerializeTOC() )
	{
		appSetSplashText( STT_StartupProgress, TEXT( "Prepare engine content" ) );
		AddTOCEntries( GetEngineContentDir() );

		appSetSplashText( STT_StartupProgress, TEXT( "Prepare game content" ) );
		AddTOCEntries( GetGameContentDir() );

		SerializeTOC( true );
	}

	Super::Init();

	// Register actor factory for assets
	GActorFactory.Register( AT_StaticMesh,	&AStaticMesh::SpawnActorAsset );
	GActorFactory.Register( AT_AudioBank,	&AAudio::SpawnActorAsset );

	// Register importers of assets
	GAssetFactory.RegisterImporter( &CTexture2DImporter::Import, &CTexture2DImporter::Reimport, CTexture2DImporter::GetSupportedExtensions(), AT_Texture2D );

	// Create window and main viewport
	uint32						windowWidth		= GConfig.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight	= GConfig.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();

	GWindow->SetTitle( GetEditorName().c_str() );
	GWindow->SetSize( windowWidth, windowHeight );

	editorInterfaceViewportClient	= new CEditorInterfaceViewportClient();
	CViewport*	viewport			= new CViewport();
	viewport->SetViewportClient( editorInterfaceViewportClient );
	viewport->Update( false, windowWidth, windowHeight, GWindow->GetHandle() );
	viewports.push_back( viewport );

	// Init all windows
	CImGUILayer*	imguiLayers[] =
	{
		&actorClassesWindow,
		&actorPropertiesWindow,
		&contentBrowserWindow,
		&explorerLevelWindow,
		&logsWindow,
		&viewportWindows[LVT_OrthoXY],
		&viewportWindows[LVT_OrthoXZ],
		&viewportWindows[LVT_OrthoYZ],
		&viewportWindows[LVT_Perspective]
	};

	for ( uint32 index = 0; index < ARRAY_COUNT( imguiLayers ); ++index )
	{
		imguiLayers[index]->Init();
	}
}

void CEditorEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );

	// Update viewports
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[index]->Tick( InDeltaSeconds );
	}
	
	// Reset input events after engine tick
	GInputSystem->ResetEvents();

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	for ( int32 index = viewports.size()-1; index >= 0; --index )
	{
		viewports[index]->Draw();
	}
}

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
	GEditorEngine = nullptr;
}

void CEditorEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	Super::ProcessEvent( InWindowEvent );

	switch ( InWindowEvent.type )
	{
	case SWindowEvent::T_WindowClose:
		if ( InWindowEvent.events.windowClose.windowId == GWindow->GetID() )
		{
			GIsRequestingExit = true;
		}
		break;

	case SWindowEvent::T_WindowResize:
		if ( InWindowEvent.events.windowResize.windowId == GWindow->GetID() )
		{
			viewports[0]->Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, GWindow->GetHandle() );
		}
		break;
	}
}

void CEditorEngine::PrintLogToWidget( ELogType InLogType, const tchar* InMessage )
{}

bool CEditorEngine::LoadMap( const std::wstring& InMap, std::wstring& OutError )
{
	if ( !Super::LoadMap( InMap, OutError ) )
	{
		LE_LOG( LT_Warning, LC_General, TEXT( "Failed loading map '%s'. Error: %s" ), InMap.c_str(), OutError.c_str() );
	}

	SEditorDelegates::onEditorLoadedMap.Broadcast();
	return true;
}

bool CEditorEngine::SaveMap( const std::wstring& InMap, std::wstring& OutError )
{
	CArchive*	arWorld = GFileSystem->CreateFileWriter( InMap );
	if ( !arWorld )
	{
		OutError = TEXT( "Failed open archive" );
		return false;
	}

	arWorld->SetType( AT_World );
	arWorld->SerializeHeader();
	GWorld->Serialize( *arWorld );
	delete arWorld;

	return true;
}

void CEditorEngine::AddTOCEntries( const std::wstring& InRootDir )
{
	std::vector< std::wstring >		files = GFileSystem->FindFiles( InRootDir, true, true );
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
			GTableOfContents.AddEntry( fullPath );
		}
	}
}

bool CEditorEngine::SerializeTOC( bool InIsSave /* = false */ )
{
	std::wstring	pathTOC = appGameDir() + ( PATH_SEPARATOR TEXT( "EditorCache" ) ) + PATH_SEPARATOR + GTableOfContents.GetNameTOC();
	CArchive* archiveTOC = !InIsSave ? GFileSystem->CreateFileReader( pathTOC ) : GFileSystem->CreateFileWriter( pathTOC );
	if ( archiveTOC )
	{
		GTableOfContents.Serialize( *archiveTOC );
		delete archiveTOC;
		return true;
	}
	else
	{
		LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed serialize TOC file '%s'" ), pathTOC.c_str() );
		return false;
	}
}

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
	
	return CString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );
}