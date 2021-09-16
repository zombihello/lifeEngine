#include "Widgets/InputText.h"

WInputText::WInputText( const tchar* InLabel, uint32 InMaxSize /* = 256 */ ) :
	label( TCHAR_TO_ANSI( InLabel ) ),
	enteredText( InMaxSize, '\0' )
{}

void WInputText::Tick()
{
	WBaseWidget::Tick();

	ImGui::Text( label.c_str() );
	ImGui::SameLine();
	ImGui::InputText( ( std::string( "##" ) + label ).c_str(), ( char* )enteredText.data(), enteredText.size() );
}