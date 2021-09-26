#include "WorldEdApp.h"

class WxTest : public wxFrame
{
public:
	WxTest( const wxString& title ) :
		wxFrame( nullptr, wxID_ANY, title, wxDefaultPosition, wxSize( 250, 150 ) )
	{
		Centre();
	}
};

bool WxWorldEdApp::OnInit()
{
	WxLaunchApp::OnInit();

	WxTest* t = new WxTest( wxT( "SSSS" ) );
	t->Show();

	return true;
}