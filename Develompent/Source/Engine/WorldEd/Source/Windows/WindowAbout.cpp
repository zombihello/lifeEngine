#include "Containers/String.h"
#include "Windows/WindowAbout.h"
#include "WorldEdApp.h"
#include "Core.h"

WxWindowAbout::WxWindowAbout( wxWindow* InParent ) :
	wxDialog( InParent, wxID_ANY, FString::Format( TEXT( "About %s" ), WxWorldEdApp::GetEditorName().c_str() ), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE )
{
	Center();
}

WxWindowAbout::~WxWindowAbout()
{}

void WxWindowAbout::OnClose( wxCloseEvent& InEvent )
{
	Destroy();
}

void WxWindowAbout::Create( wxWindow* InParent )
{
	WxWindowAbout*		windowAbout = new WxWindowAbout( InParent );
	windowAbout->Show();
}

//----------------------------------------------------
//   Events of WxWindowAbout
//----------------------------------------------------

BEGIN_EVENT_TABLE( WxWindowAbout, wxDialog )
	EVT_CLOSE( WxWindowAbout::OnClose )
END_EVENT_TABLE()