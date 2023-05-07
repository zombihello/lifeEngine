#include "Containers/StringConv.h"
#include "Windows/DialogWindow.h"

/** Table of buttons name by flag */
static const tchar* s_ButtonsText[] =
{
	TEXT( "Ok" ),			// BT_Ok
	TEXT( "Cancel" ),		// BT_Cancel
	TEXT( "Yes" ),			// BT_Yes
	TEXT( "Yes To All" ),	// BT_YesToAll
	TEXT( "No" ),			// BT_No
	TEXT( "No To All" )		// BT_NoToAll
};


/*
==================
CDialogWindow::CDialogWindow
==================
*/
CDialogWindow::CDialogWindow( const std::wstring& InName, const std::wstring& InMessage, uint32 InButtons /* = BT_Ok */ )
	: CImGUIPopup( InName )
	, buttons( InButtons )
	, message( InMessage )
{
	SetSize( Vector2D( 500, 300 ) );
}

/*
==================
CDialogWindow::OnTick
==================
*/
void CDialogWindow::OnTick()
{
	// Draw text message
	ImGui::TextWrapped( TCHAR_TO_ANSI( message.c_str() ) );
	ImGui::Separator();

	// Draw buttons
	for ( uint32 index = 0, count = ARRAY_COUNT( s_ButtonsText ); index < count; ++index )
	{
		if ( !( buttons & 1 << index ) )
		{
			continue;
		}

		if ( index > 0 )
		{
			ImGui::SameLine();
		}

		if ( ImGui::Button( TCHAR_TO_ANSI( s_ButtonsText[index] ), ImVec2{ 120.f, 0.f } ) )
		{
			onButtonPressed.Broadcast( ( EButtonType )( 1 << index ) );
			Close();
		}	
	}
}