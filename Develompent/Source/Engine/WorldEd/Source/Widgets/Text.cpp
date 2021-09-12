#include "ImGUI/ImGUIEngine.h"
#include "Widgets/Text.h"

WText::WText( const tchar* InText /* = TEXT( "" ) */ ) :
	text( TCHAR_TO_ANSI( InText ) )
{}

void WText::Tick()
{
	check( IsInitialized() );
	ImGui::Text( text.c_str() );
}