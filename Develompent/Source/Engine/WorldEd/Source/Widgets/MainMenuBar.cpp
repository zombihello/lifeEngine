#include "ImGUI/ImGUIEngine.h"
#include "Widgets/MainMenuBar.h"

//
// Definitions
//

/** Default name for menu */
#define DEFAULT_NAME			"None"

//
// ITEM OF MENU BAR
//

WMainMenuBar::FMenuItem::FMenuItem( const tchar* InName, FCallbackFunction InCallback ) :
	name( TCHAR_TO_ANSI( InName ) ),
	callback( InCallback )
{}

void WMainMenuBar::FMenuItem::Tick()
{
	if ( ImGui::MenuItem( name.c_str() ) )
	{
		if ( callback )
		{
			callback();
		}
	}
}

//
// MENU BAR
//

WMainMenuBar::FMenu::FMenu() :
	name( DEFAULT_NAME )
{}

WMainMenuBar::FMenu::FMenu( const tchar* InName ) :
	name( TCHAR_TO_ANSI( InName ) )
{}

TRefCountPtr< WMainMenuBar::FMenuItem > WMainMenuBar::FMenu::FindItem( const tchar* InName ) const
{
	for ( uint32 index = 0, count = ( uint32 )menuItems.size(); index < count; ++index )
	{
		FMenuItem*			menuItem = menuItems[ index ];
		if ( menuItem->GetName() == InName )
		{
			return menuItem;
		}
	}

	return nullptr;
}

void WMainMenuBar::FMenu::Tick()
{
	if ( ImGui::BeginMenu( name.c_str() ) )
	{
		for ( uint32 index = 0, count = ( uint32 ) menuItems.size(); index < count; ++index )
		{
			menuItems[ index ]->Tick();
		}

		ImGui::EndMenu();
	}
}

//
// WIDGET MAIN MENU BAR
//

TRefCountPtr< WMainMenuBar::FMenu > WMainMenuBar::FindMenu( const tchar* InName ) const
{
	for ( uint32 index = 0, count = ( uint32 )menus.size(); index < count; ++index )
	{
		FMenu*			menu = menus[ index ];
		if ( menu->GetName() == InName )
		{
			return menu;
		}
	}

	return nullptr;
}

void WMainMenuBar::Tick()
{
	if ( ImGui::BeginMainMenuBar() )
	{
		for ( uint32 index = 0, count = ( uint32 ) menus.size(); index < count; ++index )
		{
			menus[ index ]->Tick();
		}

		ImGui::EndMainMenuBar();
	}
}