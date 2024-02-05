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
#include "Engine/System/InputSystem.h"
#include "UI/ImGUI/imgui_stdlib.h"
#include "Windows/InputTextDialog.h"

/*
==================
CInputTextDialog::CInputTextDialog
==================
*/
CInputTextDialog::CInputTextDialog( const std::wstring& InName, const std::wstring& InMessage, const std::wstring& InDefaultText /* = TEXT( "" ) */ )
	: CImGUIPopup( InName )
	, message( InMessage )
	, text( TCHAR_TO_ANSI( InDefaultText.c_str() ) )
{}

/*
==================
CInputTextDialog::OnTick
==================
*/
void CInputTextDialog::OnTick()
{
	// Draw text message and input field
	ImGui::TextWrapped( TCHAR_TO_ANSI( message.c_str() ) );
	ImGui::Separator();
	ImGui::InputText( "", &text );
	ImGui::Separator();

	// Draw buttons
	const ImVec2		buttonSize( 120.f, 0.f );
	if ( ImGui::Button( "Ok", buttonSize ) || g_InputSystem->IsKeyDown( BC_KeyEnter ) )
	{
		onTextEntered.Broadcast( text );
		Close();
	}

	ImGui::SameLine();
	if ( ImGui::Button( "Cancel", buttonSize ) )
	{
		onCenceled.Broadcast();
		Close();
	}
}