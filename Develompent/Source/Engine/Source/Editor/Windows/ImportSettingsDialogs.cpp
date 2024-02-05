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

#include "UI/ImGUI/imgui_internal.h"
#include "Editor/Windows/ImportSettingsDialogs.h"

/** Table names of axis up */
static const achar* s_AxisUpNames[] =
{
	"+X",	// AU_PlusX
	"-X",	// AU_MinusX
	"+Y",	// AU_PlusY
	"-Y",	// AU_MinusY
	"+Z",	// AU_PlusZ
	"-Z"	// AU_MinusZ
};
static_assert( ARRAY_COUNT( s_AxisUpNames ) == CStaticMeshImportSettingsDialog::AU_Num, "Need full init s_AxisUpNames array" );

/*
==================
CStaticMeshImportSettingsDialog::CStaticMeshImportSettingsDialog
==================
*/
CStaticMeshImportSettingsDialog::CStaticMeshImportSettingsDialog()
	: CImGUIPopup( TEXT( "Static Mesh Import Settings" ) )
{
	SetSize( Vector2D( 125.f, 700.f ) );
}

/*
==================
CStaticMeshImportSettingsDialog::OnTick
==================
*/
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
			if ( ImGui::Combo( "##AxisUp", &axisUp, s_AxisUpNames, ARRAY_COUNT( s_AxisUpNames ) ) )
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