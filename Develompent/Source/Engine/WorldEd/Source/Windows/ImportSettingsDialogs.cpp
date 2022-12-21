#include "Windows/ImportSettingsDialogs.h"
#include "ImGUI/imgui_internal.h"

/** Table names of axis up */
static const achar* GAxisUpNames[] =
{
	"+X",	// AU_PlusX
	"-X",	// AU_MinusX
	"+Y",	// AU_PlusY
	"-Y",	// AU_MinusY
	"+Z",	// AU_PlusZ
	"-Z"	// AU_MinusZ
};
static_assert( ARRAY_COUNT( GAxisUpNames ) == CStaticMeshImportSettingsDialog::AU_Num, "Need full init GAxisUpNames array" );

CStaticMeshImportSettingsDialog::CStaticMeshImportSettingsDialog()
	: CImGUIPopup( TEXT( "Static Mesh Import Settings" ) )
{
	SetSize( Vector2D( 125.f, 700.f ) );
}

void CStaticMeshImportSettingsDialog::OnTick()
{
	// Mesh section
	if ( ImGui::CollapsingHeader( "Mesh", ImGuiTreeNodeFlags_DefaultOpen ) )
	{
		ImGui::Columns( 2, 0, false );

		// Combine meshes
		{
			ImGui::Text( "Combine Meshes:" );
			if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
			{
				ImGui::SetTooltip( "If enabled, combines all meshes into a single mesh" );
			}

			ImGui::NextColumn();
			ImGui::Checkbox( "##CombineMeshes", &importSettings.bCombineMeshes );
			ImGui::NextColumn();
		}

		// Axis up in mesh
		{
			ImGui::Text( "Axis Up:" );
			if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenDisabled ) )
			{
				ImGui::SetTooltip( "Axis up in mesh" );
			}

			ImGui::NextColumn();
			int32	axisUp = importSettings.axisUp;
			if ( ImGui::Combo( "##AxisUp", &axisUp, GAxisUpNames, ARRAY_COUNT( GAxisUpNames ) ) )
			{
				importSettings.axisUp = ( EAxisUp )axisUp;
			}
		}
		ImGui::EndColumns();
	}

	// Draw buttons
	ImGui::NewLine();
	ImGui::Separator();

	// Import asset
	bool				bNeedClose = false;
	const ImVec2		buttonSize( 120.f, 0.f );
	if ( ImGui::Button( "Import", buttonSize ) )
	{
		onResume.Broadcast( CAssetFactory::RSIS_Import, importSettings );
		bNeedClose = true;
	}
	
	// Import all assets
	ImGui::SameLine();
	if ( ImGui::Button( "Import All", buttonSize ) )
	{
		onResume.Broadcast( CAssetFactory::RSIS_ImportAll, importSettings );
		bNeedClose = true;
	}

	// Cancel
	ImGui::SameLine();
	if ( ImGui::Button( "Cancel", buttonSize ) )
	{
		onResume.Broadcast( CAssetFactory::RSIS_Cancel, importSettings );
		bNeedClose = true;
	}

	if ( bNeedClose )
	{
		Close();
	}
}