#include "Containers/StringConv.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "Render/EditorInterfaceViewportClient.h"
#include "System/EditorEngine.h"
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_internal.h"
#include "UIEngine.h"
#include "Render/Material.h"
#include "Render/Texture.h"
#include "System/Package.h"
void CEditorInterfaceViewportClient::Draw( CViewport* InViewport )
{
	GUIEngine->BeginDraw();
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
			ImGuiID		dockIdRight				= ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Right, 0.15f, nullptr, &dockspaceId );
			ImGuiID		dockIdDown				= ImGui::DockBuilderSplitNode( dockIdRight, ImGuiDir_Down, 0.5f, nullptr, &dockIdRight );
			
			ImGuiID		dockIdViewportXY		= ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Up, 0.5f, nullptr, &dockspaceId );
			ImGuiID		dockIdContentBrowser	= ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Down, 0.2f, nullptr, &dockspaceId );
			ImGuiID		dockIdViewportXZ		= ImGui::DockBuilderSplitNode( dockIdViewportXY, ImGuiDir_Right, 0.5f, nullptr, &dockIdViewportXY );
			ImGuiID		dockIdViewportYZ		= ImGui::DockBuilderSplitNode( dockspaceId, ImGuiDir_Right, 0.5f, nullptr, &dockspaceId );

			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetExplorerLevelWindow()->GetName().c_str() ), dockIdRight );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetActorPropertiesWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetContentBrowserWindow()->GetName().c_str() ), dockIdContentBrowser );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetActorClassesWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetLogsWindow()->GetName().c_str() ), dockIdDown );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetLevelViewportWindow( LVT_Perspective )->GetName().c_str() ), dockspaceId );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetLevelViewportWindow( LVT_OrthoXY )->GetName().c_str() ), dockIdViewportXY );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetLevelViewportWindow( LVT_OrthoXZ )->GetName().c_str() ), dockIdViewportXZ );
			ImGui::DockBuilderDockWindow( TCHAR_TO_ANSI( GEditorEngine->GetLevelViewportWindow( LVT_OrthoYZ )->GetName().c_str() ), dockIdViewportYZ );
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

	GUIEngine->EndDraw();
}