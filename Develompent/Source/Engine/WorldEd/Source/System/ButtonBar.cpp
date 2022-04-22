#include "System/ButtonBar.h"

bool WxButtonBar::Create( wxWindow* InParent, wxWindowID InID, const wxPoint& InPosition /* = wxDefaultPosition */, const wxSize& InSize /* = wxDefaultSize */, uint64 InStyle /* = wxTAB_TRAVERSAL */, const std::wstring& InName /* = TEXT( "Panel" ) */ )
{
	bool		bResult = wxPanel::Create( InParent, InID, InPosition, InSize, InStyle, InName.c_str() );
	return bResult;
}