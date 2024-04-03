#include "Misc/StringConv.h"
#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "Windows/InputTextDialog.h"
#include "ImGUI/imgui_stdlib.h"

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