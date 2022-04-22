#include "System/Controls.h"

//----------------------------------------------------
//   WxStatusBar
//----------------------------------------------------

WxStatusBar::WxStatusBar()
	: wxStatusBar()
{}

WxStatusBar::WxStatusBar( wxWindow* InParent, wxWindowID InID )
	: wxStatusBar( InParent, InID )
{}

WxStatusBar::~WxStatusBar()
{}

void WxStatusBar::SetStatusText( const wxString& InText, uint32 InIndex /* = 0 */ )
{
	wxStatusBar::SetStatusText( InText, InIndex );
}

void WxStatusBar::SetMouseWorldspacePositionText( const std::wstring& InStatusText )
{}