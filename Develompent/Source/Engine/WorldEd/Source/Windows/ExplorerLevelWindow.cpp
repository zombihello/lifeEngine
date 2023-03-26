#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "System/InputSystem.h"
#include "Windows/ExplorerLevelWindow.h"

CExplorerLevelWindow::CExplorerLevelWindow( const std::wstring& InName )
	: CImGUILayer( InName )
{}

void CExplorerLevelWindow::OnTick()
{
	// Static constant colors of row background in a table
	static const uint32		STableBgColor0		= CColor( 26.f, 27.f, 28.f ).GetUInt32Color();
	static const uint32		STableBgColor1		= CColor( 32.f, 35.f, 36.f ).GetUInt32Color();
	static const ImVec4		STableBgSelectColor = ImVec4( 0.f, 0.43f, 0.87f, 1.f );

	if ( ImGui::BeginTable( "##ExploerLevelTable", 3, ImGuiTableFlags_Resizable ) )
	{
		// Init header table
		ImGui::TableSetupColumn( "Visibility" );
		ImGui::TableSetupColumn( "Label" );
		ImGui::TableSetupColumn( "Class" );
		ImGui::TableHeadersRow();

		// Init style
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( ImGui::GetStyle().FramePadding.x, 2.f ) );
		ImGui::PushStyleColor( ImGuiCol_Header, STableBgSelectColor );

		// Print info about each actor on level
		for ( uint32 index = 0, numActors = GWorld->GetNumActors(); index < numActors; ++index )
		{
			ActorRef_t		actor = GWorld->GetActor( index );
			check( actor );
			bool			bNewVisibility = actor->IsVisibility();
			bool			bSelected = actor->IsSelected();

			ImGui::TableNextRow();
			ImGui::TableSetBgColor( ImGuiTableBgTarget_RowBg0, !( index % 2 ) ? STableBgColor0 : STableBgColor1 );
			
			// Visibility flag
			ImGui::TableNextColumn();
			if ( ImGui::Checkbox( TCHAR_TO_ANSI( CString::Format( TEXT( "##Visibility_ID_%i" ), index ).c_str() ), &bNewVisibility ) )
			{
				actor->SetVisibility( bNewVisibility );
			}
			
			// Label
			ImGui::TableNextColumn();
			ImGui::Text( TCHAR_TO_ANSI( actor->GetName() ) );		

			// Class name
			ImGui::TableNextColumn();
			ImGui::Text( TCHAR_TO_ANSI( actor->GetClass()->GetName().c_str() ) );

			// Selectable widget
			ImGui::SameLine();
			{
				const bool	bCtrlDown			= GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );
				bool		bNeedPopStyleColor	= false;

				if ( bSelected )
				{
					ImGui::PushStyleColor( ImGuiCol_HeaderHovered, STableBgSelectColor );
					bNeedPopStyleColor = true;
				}
				if ( ImGui::Selectable( TCHAR_TO_ANSI( CString::Format( TEXT( "##Selectable_ID_%i" ), index ).c_str() ), &bSelected, ImGuiSelectableFlags_SpanAllColumns ) )
				{
					bool	bUnselectedAllActors = false;
					if ( !bCtrlDown )
					{
						GWorld->UnselectAllActors();
						bUnselectedAllActors = true;
					}

					if ( bSelected || ( bUnselectedAllActors && !bCtrlDown ) )
					{
						GWorld->SelectActor( actor );
					}
					else
					{
						GWorld->UnselectActor( actor );
					}
				}
				if ( bNeedPopStyleColor )
				{
					ImGui::PopStyleColor();
				}
			}
		}

		ImGui::PopStyleVar();
		ImGui::PopStyleColor();
		ImGui::EndTable();
	}

	// Reset selection if clicked not on item
	if ( ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) )
	{
		GWorld->UnselectAllActors();
	}
}