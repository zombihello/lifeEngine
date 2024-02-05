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

#include "Core/Containers/StringConv.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/ConsoleSystem.h"
#include "UI/Misc/UIGlobals.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "UI/ImGUI/imgui_stdlib.h"
#include "Windows/LogsWindow.h"

/*
==================
CLogsWindow::CLogsWindow
==================
*/
CLogsWindow::CLogsWindow( const std::wstring& InName )
	: CImGUILayer( InName )
{}

/*
==================
CLogsWindow::OnTick
==================
*/
void CLogsWindow::OnTick()
{
	// Draw log history
	{
		// Reserve enough left-over height for 1 separator + 1 input text
		const float		footerHeightToReserve	= ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::BeginChild( "##ScrollingRegion", ImVec2( 0, -footerHeightToReserve ), false, ImGuiWindowFlags_HorizontalScrollbar );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 4.f, 1.f ) );			// Tighten spacing
		
		for ( uint32 index = 0, count = history.size(); index < count; ++index )
		{
			bool				bHasColor = false;
			const LogInfo&		logInfo = history[index];
			switch ( logInfo.type )
			{
			case LT_Warning:
				ImGui::PushStyleColor( ImGuiCol_Text, g_ImGUIEngine->GetStyleColor( IGC_WarningColor ) );
				bHasColor = true;
				break;

			case LT_Error:
				ImGui::PushStyleColor( ImGuiCol_Text, g_ImGUIEngine->GetStyleColor( IGC_ErrorColor ) );
				bHasColor = true;
				break;
			}

			ImGui::TextUnformatted( logInfo.message.c_str() );
			
			if ( bHasColor )
			{
				ImGui::PopStyleColor();
			}
		}

		if ( ImGui::GetScrollY() >= ImGui::GetScrollMaxY() )
		{
			ImGui::SetScrollHereY( 1.0f );
		}

		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		// Command-line
		bool	bReclaimFocus		= false;
		float	panelWidth			= ImGui::GetContentRegionAvail().x;
		float	inputCommandWidth	= panelWidth / 1.15f;
		ImGui::PushItemWidth( inputCommandWidth );
		if ( ImGui::InputTextWithHint( "##InputCommand", "Console Command", &commandBuffer, ImGuiInputTextFlags_EnterReturnsTrue ) )
		{
			if ( !commandBuffer.empty() )
			{
				ExecCommand( commandBuffer );
				commandBuffer.clear();
			}
			bReclaimFocus = true;
		}
		ImGui::PopItemWidth();

		// Auto-focus on window apparition
		ImGui::SetItemDefaultFocus();
		if ( bReclaimFocus )
		{
			ImGui::SetKeyboardFocusHere( -1 );		// Auto focus previous widget
		}

		ImGui::SameLine();
		if ( ImGui::Button( "Enter", ImVec2( panelWidth - inputCommandWidth, 0.f ) ) )
		{
			ExecCommand( commandBuffer );
			commandBuffer.clear();
		}
	}
}

/*
==================
CLogsWindow::ExecCommand
==================
*/
void CLogsWindow::ExecCommand( const std::string& InCommand )
{
	if ( !InCommand.empty() )
	{
		g_ConsoleSystem.Exec( ANSI_TO_TCHAR( InCommand.c_str() ) );
	}
}