#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "Windows/EditorWindow.h"
#include "Windows/FileDialog.h"
#include "Windows/DialogWindow.h"
#include "System/EditorEngine.h"
#include "System/BaseFileSystem.h"
#include "System/BaseWindow.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "UIEngine.h"
#include "System/Package.h"
#include "System/World.h"

/*
==================
CEditorWindow::CEditorWindow
==================
*/
CEditorWindow::CEditorWindow()
	: CImGUILayer( TEXT( "WorldEd" ) )
	, dockspaceFlags( ImGuiDockNodeFlags_None )
	, editorCreatedNewMapDelegate( nullptr )
	, editorLoadedMapDelegate( nullptr )
	, editorSavedMapDelegate( nullptr )
	, editorMapMarkedDirtyDelegate( nullptr )
{
	flags |= ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | 
			 ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | 
			 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	if ( dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode )
	{
		flags |= ImGuiWindowFlags_NoBackground;
	}

	// Bind to events when map created, loaded, saved and marked dirty
	editorCreatedNewMapDelegate			= SEditorDelegates::onEditorCreatedNewMap.Add( std::bind( &CEditorWindow::UpdateWindowTitle, this ) );
	editorLoadedMapDelegate				= SEditorDelegates::onEditorLoadedMap.Add( std::bind( &CEditorWindow::UpdateWindowTitle, this ) );
	editorSavedMapDelegate				= SEditorDelegates::onEditorSavedMap.Add( std::bind( &CEditorWindow::UpdateWindowTitle, this ) );
	editorMapMarkedDirtyDelegate		= SEditorDelegates::onEditorMapMarkedDirty.Add( std::bind( &CEditorWindow::UpdateWindowTitle, this ) );
}

/*
==================
CEditorWindow::~CEditorWindow
==================
*/
CEditorWindow::~CEditorWindow()
{
	SEditorDelegates::onEditorCreatedNewMap.Remove( editorCreatedNewMapDelegate );
	SEditorDelegates::onEditorLoadedMap.Remove( editorLoadedMapDelegate );
	SEditorDelegates::onEditorSavedMap.Remove( editorSavedMapDelegate );
	SEditorDelegates::onEditorMapMarkedDirty.Remove( editorMapMarkedDirtyDelegate );
}

/*
==================
CEditorWindow::Init
==================
*/
void CEditorWindow::Init()
{
	CImGUILayer::Init();
	SetRounding( 0.f );
	SetBorderSize( 0.f );
}

/*
==================
CEditorWindow::UpdateWindowTitle
==================
*/
void CEditorWindow::UpdateWindowTitle()
{
	g_Window->SetTitle( CString::Format( TEXT( "%s%s - %s" ), g_World->IsDirty() ? TEXT( "*" ) : TEXT( "" ), g_World->GetName().c_str(), g_EditorEngine->GetEditorName().c_str()).c_str());
}

/*
==================
CEditorWindow::OnTick
==================
*/
void CEditorWindow::OnTick()
{
	const ImGuiViewport*	imguiViewport	= ImGui::GetMainViewport();
	ImGuiIO&				imguiIO			= ImGui::GetIO();

	// Init position and size of window
	SetPosition( Vector2D( imguiViewport->WorkPos.x, imguiViewport->WorkPos.y ) );
	SetSize( Vector2D( imguiViewport->WorkSize.x, imguiViewport->WorkSize.y ) );
	SetViewportID( imguiViewport->ID );

	// Create dockspace and init him
	if ( imguiIO.ConfigFlags & ImGuiConfigFlags_DockingEnable )
	{
		ImGuiID			dockspaceId = ImGui::GetID( "WorldEd_DockSpace" );
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
			ImGuiID		dockIdRight = ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Right, 0.15f, nullptr, &dockspaceId );
			ImGuiID		dockIdDown = ImGui::DockBuilderSplitNode( dockIdRight, ImGuiDir_Down, 0.5f, nullptr, &dockIdRight );

			ImGuiID		dockIdViewportXY = ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Up, 0.5f, nullptr, &dockspaceId );
			ImGuiID		dockIdContentBrowser = ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Down, 0.2f, nullptr, &dockspaceId );
			ImGuiID		dockIdViewportXZ = ImGui::DockBuilderSplitNode( dockIdViewportXY, ImGuiDir_Right, 0.5f, nullptr, &dockIdViewportXY );
			ImGuiID		dockIdViewportYZ = ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Right, 0.5f, nullptr, &dockspaceId );

			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetExplorerLevelWindow()->GetName().c_str() ), dockIdRight );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetActorPropertiesWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetContentBrowserWindow()->GetName().c_str() ), dockIdContentBrowser );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetActorClassesWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetLogsWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetLevelViewportWindow( LVT_Perspective )->GetName().c_str() ), dockspaceId );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetLevelViewportWindow( LVT_OrthoXY )->GetName().c_str() ), dockIdViewportXY );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetLevelViewportWindow( LVT_OrthoXZ )->GetName().c_str() ), dockIdViewportXZ );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( g_EditorEngine->GetLevelViewportWindow( LVT_OrthoYZ )->GetName().c_str() ), dockIdViewportYZ );
			ImGui::DockBuilderFinish( dockspaceId );
		}
	}

	if ( ImGui::BeginMenuBar() )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			// New level
			if ( ImGui::MenuItem( "New Level" ) )
			{
				// If map is dirty we ask user would he sure want to create a new map
				if ( g_World->IsDirty() )
				{
					CDialogWindow::EButtonType	pressedButton;
					TSharedPtr<CDialogWindow>	popup = OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "Map not saved, all changes will be lost.\nAre you sure you want to create a new level?" ) ), CDialogWindow::BT_Ok | CDialogWindow::BT_Cancel );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  if ( InButtonType == CDialogWindow::BT_Ok )
													  {
														  g_EditorEngine->NewMap();
													  }
												  } );
				}
				// Otherwise we simple creating new map
				else
				{
					
					g_EditorEngine->NewMap();
				}
			}

			// Open level
			if ( ImGui::MenuItem( "Open Level" ) )
			{
				// If map is dirty we ask user would he sure want to open other map
				if ( g_World->IsDirty() )
				{
					CDialogWindow::EButtonType	pressedButton;
					TSharedPtr<CDialogWindow>	popup = OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "Map not saved, all changes will be lost.\nAre you sure you want to open other a level?" ) ), CDialogWindow::BT_Ok | CDialogWindow::BT_Cancel );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  if ( InButtonType == CDialogWindow::BT_Ok )
													  {
														  OpenLevel();
													  }
												  } );
				}
				// Otherwise we simple opening level
				else
				{
					OpenLevel();
				}
			}
			ImGui::Separator();

			// Save level
			if ( ImGui::MenuItem( "Save Level" ) )
			{
				SaveLevel();
			}

			// Save level as
			if ( ImGui::MenuItem( "Save Level As..." ) )
			{
				SaveLevel( true );
			}
			ImGui::Separator();

			// Exit
			if ( ImGui::MenuItem( "Exit" ) )
			{
				// If map is dirty we ask user would he sure want to exit from WorldEd
				if ( g_World->IsDirty() )
				{
					CDialogWindow::EButtonType	pressedButton;
					TSharedPtr<CDialogWindow>	popup = OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "Map not saved, all changes will be lost.\nAre you sure you want to exit from WorldEd?" ) ), CDialogWindow::BT_Ok | CDialogWindow::BT_Cancel );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  if ( InButtonType == CDialogWindow::BT_Ok )
													  {
														  Sys_RequestExit( false );
													  }
												  } );
				}
				// Otherwise we simple exit from WorldEd
				else
				{
					Sys_RequestExit( false );
				}
			}

			ImGui::EndMenu();
		}
		if ( ImGui::BeginMenu( "View" ) )
		{
			TSharedPtr<CImGUILayer>				imguiLayers[] =
			{
				g_EditorEngine->GetActorClassesWindow(),
				g_EditorEngine->GetActorPropertiesWindow(),
				g_EditorEngine->GetContentBrowserWindow(),
				g_EditorEngine->GetExplorerLevelWindow(),
				g_EditorEngine->GetLogsWindow()
			};
			
			for ( uint32 index = 0; index < ARRAY_COUNT( imguiLayers ); ++index )
			{
				const TSharedPtr<CImGUILayer>&	imguiLayer	= imguiLayers[index];
				bool							bVisibility = imguiLayer->IsVisibility();
				if ( ImGui::MenuItem( TCHAR_TO_ANSI( imguiLayer->GetName().c_str() ), nullptr, &bVisibility ) )
				{
					imguiLayer->SetVisibility( bVisibility );
				}
			}

			// Viewports
			if ( ImGui::BeginMenu( "Viewports" ) )
			{
				for ( uint32 index = 0; index < LVT_Max; ++index )
				{
					TSharedPtr<CImGUILayer> imguiLayer	= g_EditorEngine->GetLevelViewportWindow( ( ELevelViewportType )index );
					bool					bVisibility = imguiLayer->IsVisibility();
					if ( ImGui::MenuItem( TCHAR_TO_ANSI( imguiLayer->GetName().c_str() ), nullptr, &bVisibility ) )
					{
						imguiLayer->SetVisibility( bVisibility );
					}
				}
				ImGui::EndMenu();
			}
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
}

/*
==================
CEditorWindow::OpenLevel
==================
*/
void CEditorWindow::OpenLevel()
{
	// Init file dialog settings
	CFileDialogSetup		fileDialogSetup;
	SOpenFileDialogResult	openFileDialogResult;

	fileDialogSetup.SetTitle( TEXT( "Open Level" ) );
	fileDialogSetup.SetDirectory( Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content" ) );
	fileDialogSetup.SetDefaultExtension( TEXT( "map" ) );
	fileDialogSetup.AddFormat( TEXT( "*.map" ), TEXT( "lifeEngine Map" ) );

	// Open other map
	if ( Sys_ShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) )
	{
		Assert( !openFileDialogResult.files.empty() );

		std::wstring		error;
		if ( !g_EditorEngine->LoadMap( openFileDialogResult.files[0], error ) )
		{
			OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The level %s wasn't open.\n\nError: %s" ), openFileDialogResult.files[0].c_str(), error.c_str() ), CDialogWindow::BT_Ok );
		}
	}
}

/*
==================
CEditorWindow::SaveLevel
==================
*/
void CEditorWindow::SaveLevel( bool InIsSaveAs /* = false */ )
{
	CFileDialogSetup		fileDialogSetup;
	SSaveFileDialogResult	saveFileDialogResult;

	// Init file dialog settings
	fileDialogSetup.SetTitle( TEXT( "Save Level" ) );
	fileDialogSetup.SetDirectory( Sys_GameDir() + PATH_SEPARATOR + TEXT( "Content" ) );
	fileDialogSetup.SetDefaultExtension( TEXT( "map" ) );
	fileDialogSetup.AddFormat( TEXT( "*.map" ), TEXT( "lifeEngine Map" ) );

	// Save level, if file path isn't we show save file dialog
	if ( !InIsSaveAs && !g_World->GetFilePath().empty() || Sys_ShowSaveFileDialog( fileDialogSetup, saveFileDialogResult ) )
	{
		// Get file path to level file
		std::wstring		filePath = g_World->GetFilePath();
		if ( InIsSaveAs || filePath.empty() )
		{
			Assert( !saveFileDialogResult.files.empty() );
			filePath = saveFileDialogResult.files[0];
			if ( CFilename( filePath ).GetExtension().empty() )
			{
				filePath += TEXT( ".map" );
			}
		}

		// Save map
		std::wstring		error;
		if ( !g_EditorEngine->SaveMap( filePath, error ) )
		{
			OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The level %s wasn't save.\n\nError: %s" ), filePath.c_str(), error.c_str() ), CDialogWindow::BT_Ok );
		}
	}
}