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
#include "UIEngine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"

// Actors
#include "Actors/PlayerStart.h"
#include "Actors/StaticMesh.h"
#include "Actors/Audio.h"

IMPLEMENT_CLASS( CEditorEngine )

CEditorEngine::CEditorEngine()
	: currentEditorMode( EM_Default )
	, actorClassesWindow( TEXT( "Classes" ) )
	, actorPropertiesWindow( TEXT( "Properties" ) )
	, contentBrowserWindow( TEXT( "Content" ) )
	, explorerLevelWindow( TEXT( "Explorer Level" ) )
	, logsWindow( TEXT( "Logs" ) )
	, sceneViewportWindow( TEXT( "Scene" ) )
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

	// Create window and main viewport
	uint32						windowWidth		= GConfig.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowWidth" ) ).GetInt();
	uint32						windowHeight	= GConfig.GetValue( CT_Engine, TEXT( "Engine.SystemSettings" ), TEXT( "WindowHeight" ) ).GetInt();

	GWindow->SetTitle( GetEditorName().c_str() );
	GWindow->SetSize( windowWidth, windowHeight );
	viewport.Update( false, windowWidth, windowHeight, GWindow->GetHandle() );
}

void CEditorEngine::Tick( float InDeltaSeconds )
{
	Super::Tick( InDeltaSeconds );

	// Update viewport
	viewport.Tick( InDeltaSeconds );

	// Reset input events after engine tick
	GInputSystem->ResetEvents();

	// Wait while render thread is rendering of the frame
	FlushRenderingCommands();

	// Draw frame to viewports
	GUIEngine->BeginDraw();
	viewport.Draw();
	TickImGUI();
	GUIEngine->EndDraw();
}

void CEditorEngine::TickImGUI()
{
	ImGuiWindowFlags		windowFlags		= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiDockNodeFlags		dockspaceFlags	= ImGuiDockNodeFlags_None;
	const ImGuiViewport*	imguiViewport	= ImGui::GetMainViewport();
	ImGuiIO&				imguiIO			= ImGui::GetIO();

	// Init dockspace window
	ImGui::SetNextWindowPos( imguiViewport->WorkPos );
	ImGui::SetNextWindowSize( imguiViewport->WorkSize );
	ImGui::SetNextWindowViewport( imguiViewport->ID );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
	ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if ( dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode )
	{
		windowFlags |= ImGuiWindowFlags_NoBackground;
	}

	// Draw editor interface
	ImGui::Begin( "WorldEd_DockSpace", nullptr, windowFlags );
	ImGui::PopStyleVar( 2 );

	// Create dockspace and init him
	if ( imguiIO.ConfigFlags & ImGuiConfigFlags_DockingEnable )
	{
		ImGuiID			dockspaceId	= ImGui::GetID( "WorldEd_DockSpace" );
		ImGui::DockSpace( dockspaceId, ImVec2( 0.0f, 0.0f ), dockspaceFlags );

		static bool		bDockInit = false;
		if ( !bDockInit )
		{
			// Add new dock node
			bDockInit = true;
			ImGui::DockBuilderRemoveNode( dockspaceId );
			ImGui::DockBuilderAddNode( dockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace );
			ImGui::DockBuilderSetNodeSize( dockspaceId, imguiViewport->Size );

			// Split dock nodes
			ImGuiID		dockIdRight = ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Right, 0.2f, nullptr, &dockspaceId );
			ImGuiID		dockIdDown = ImGui::DockBuilderSplitNode( dockIdRight, ImGuiDir_Down, 0.5f, nullptr, &dockIdRight );

			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( explorerLevelWindow.GetName().c_str() ), dockIdRight );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( actorPropertiesWindow.GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( contentBrowserWindow.GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( actorClassesWindow.GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( logsWindow.GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( sceneViewportWindow.GetName().c_str() ), dockspaceId );
			ImGui::DockBuilderFinish( dockspaceId );
		}
	}

	if ( ImGui::BeginMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "View" ) )
		{
			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "Tools" ) )
		{
			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "Help" ) )
		{
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();

	// Draw all windows
	explorerLevelWindow.Tick();
	actorPropertiesWindow.Tick();
	contentBrowserWindow.Tick();
	actorClassesWindow.Tick();
	logsWindow.Tick();
	sceneViewportWindow.Tick();
}

void CEditorEngine::Shutdown()
{
	Super::Shutdown();

	// Destroy viewport
	viewport.Update( true, 0, 0, nullptr );
	FlushRenderingCommands();

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
			viewport.Update( false, InWindowEvent.events.windowResize.width, InWindowEvent.events.windowResize.height, GWindow->GetHandle() );
		}
		break;
	}

	// Process event in viewport client
	CViewportClient*	viewportClient = viewport.GetViewportClient();
	if ( viewportClient )
	{
		viewportClient->ProcessEvent( InWindowEvent );
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