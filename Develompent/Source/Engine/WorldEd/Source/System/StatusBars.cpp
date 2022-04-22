#include "System/StatusBars.h"

void WxStatusBarStandard::SetUp()
{}

void WxStatusBarStandard::UpdateUI()
{}

void WxStatusBarStandard::RefreshPositionAndSize()
{
	wxSizeEvent		dummyEvent;
	OnSize( dummyEvent );
}

void WxStatusBarStandard::OnSize( wxSizeEvent& InEvent )
{}

//----------------------------------------------------
//   Events of WxStatusBarStandard
//----------------------------------------------------

BEGIN_EVENT_TABLE( WxStatusBarStandard, WxStatusBar )
	EVT_SIZE( WxStatusBarStandard::OnSize )
END_EVENT_TABLE()