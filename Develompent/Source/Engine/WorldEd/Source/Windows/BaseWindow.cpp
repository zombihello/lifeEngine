#include "Misc/WorldEdGlobals.h"
#include "Containers/StringConv.h"
#include "ImGUI/ImGUIEngine.h"
#include "Windows/WindowsManager.h"
#include "Windows/BaseWindow.h"
#include "CoreDefines.h"

WBaseWindow::WBaseWindow() :
	isOpen( true ),
	title( "None" )
{}

WBaseWindow::WBaseWindow( const tchar* InTitle /* = TEXT( "" ) */, uint32 InFlags /* = 0 */ ) :
	isOpen( true ),
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

	bool	result = ImGui::Begin( title.c_str(), &isOpen, ( ImGuiWindowFlags )flags );
	if ( result && isOpen )
	{
		for ( auto it = widgets.begin(), itEnd = widgets.end(); it != itEnd; ++it )
		{
			it->second->Tick();
		}
	}
	ImGui::End();

	if ( !isOpen )
	{
		Close();
	}
}