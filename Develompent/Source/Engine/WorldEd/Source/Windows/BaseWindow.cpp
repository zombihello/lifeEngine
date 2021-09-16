#include "Misc/WorldEdGlobals.h"
#include "Containers/StringConv.h"
#include "ImGUI/ImGUIEngine.h"
#include "Windows/WindowsManager.h"
#include "Windows/BaseWindow.h"
#include "CoreDefines.h"

WBaseWindow::WBaseWindow() :
	isOpen( false ),
	title( "None" )
{}

WBaseWindow::WBaseWindow( const tchar* InTitle /* = TEXT( "" ) */, uint32 InFlags /* = 0 */ ) :
	isOpen( false ),
	title( TCHAR_TO_ANSI( InTitle ) ),
	flags( InFlags )
{}

void WBaseWindow::Tick()
{
	check( IsInitialized() );
	if ( !isOpen )
	{
		return;
	}

	// If size not 0 set window size
	if ( size.x != 0.f || size.y != 0.f )
	{
		ImGui::SetNextWindowSize( ImVec2( size.x, size.y ) );
	}

	// Update window and widgets
	ImGui::BeginDisabled( !IsEnabled() );
	bool	result = ImGui::Begin( title.c_str(), &isOpen, ( ImGuiWindowFlags )flags );	
	if ( result && isOpen )
	{
		for ( auto it = widgets.begin(), itEnd = widgets.end(); it != itEnd; ++it )
		{
			( *it )->Tick();
		}
	}	
	ImGui::End();
	ImGui::EndDisabled();

	if ( !isOpen )
	{
		Close();
	}
}