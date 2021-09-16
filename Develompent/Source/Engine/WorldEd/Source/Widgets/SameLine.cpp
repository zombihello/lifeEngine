#include "ImGUI/ImGUIEngine.h"
#include "Widgets/SameLine.h"

WSameLine::WSameLine()
{}

void WSameLine::Tick()
{
	WBaseWidget::Tick();

	for ( auto it = widgets.begin(), itEnd = widgets.end(); it != itEnd; ++it )
	{
		( *it )->Tick();
		if ( std::next( it ) != widgets.end() )
		{
			ImGui::SameLine();
		}
	}
}