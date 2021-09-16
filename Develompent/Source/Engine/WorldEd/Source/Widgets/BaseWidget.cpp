#include "ImGUI/ImGUIEngine.h"
#include "Widgets/BaseWidget.h"

WBaseWidget::WBaseWidget() :
	isInitialized( false ),
	isEnabled( true )
{}

WBaseWidget::~WBaseWidget()
{}

void WBaseWidget::Init()
{
	isInitialized = true;
}

void WBaseWidget::Tick()
{
	// Set position for widget only if he setted
	if ( position.x != 0 || position.y != 0 )
	{
		ImGui::SetCursorPos( ImVec2( position.x, position.y ) );
	}
}

void WBaseWidget::ProcessEvent( struct SWindowEvent& InWindowEvent )
{}