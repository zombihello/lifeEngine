#include "Widgets/Button.h"

WButton::WButton() :
	name( "None" ),
	size( ImVec2( 0, 0 ) )
{}

WButton::WButton( const tchar* InName, const ImVec2& InSize ) :
	name( TCHAR_TO_ANSI( InName ) ),
	size( InSize )
{}

void WButton::Tick()
{
	WBaseWidget::Tick();

	check( IsInitialized() );
	if ( ImGui::Button( name.c_str(), size ) )
	{
		onButtonClicked.Broadcast( this );
	}
}