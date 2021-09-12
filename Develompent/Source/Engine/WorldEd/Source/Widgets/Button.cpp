#include "Widgets/Button.h"

WButton::WButton() :
	name( "None" ),
	size( ImVec2( 0, 0 ) ),
	callback( nullptr )
{}

WButton::WButton( const tchar* InName, const ImVec2& InSize, FCallbackFunction InCallback ) :
	name( TCHAR_TO_ANSI( InName ) ),
	size( InSize ),
	callback( InCallback )
{}

void WButton::Tick()
{
	check( IsInitialized() );
	if ( ImGui::Button( name.c_str(), size ) )
	{
		if ( callback )
		{
			callback();
		}
	}
}