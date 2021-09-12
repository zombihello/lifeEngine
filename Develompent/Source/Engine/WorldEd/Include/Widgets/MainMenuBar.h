/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MAINMENUBAR_H
#define MAINMENUBAR_H

#include <string>
#include <vector>
#include <functional>

#include "Misc/Types.h"
#include "Misc/RefCountPtr.h"
#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"

/**
 * @ingroup WorldEd
 * Class for work with main menu bar in editor
 */
class WMainMenuBar : public WBaseWidget
{
public:
	/**
	 * Class of item in menu bar
	 */
	class FMenuItem : public FRefCounted
	{
	public:
		/**
		 * Callback function type
		 */
		typedef std::function< void() >				FCallbackFunction;

		/**
		 * Constructor
		 *
		 * @param[in] InName Name menu
		 * @param[in] InCallback Function called when item triggered
		 */
		FMenuItem( const tchar* InName, FCallbackFunction InCallback );

		/**
		 * Destructor
		 */
		virtual ~FMenuItem() {}

		/**
		 * Update menu items
		 */
		void Tick();

		/**
		 * Set name
		 *
		 * @param[in] InName Name item
		 */
		FORCEINLINE void SetName( const tchar* InName )
		{
			name = TCHAR_TO_ANSI( InName );
		}

		/**
		 * Get name
		 *
		 * @return Return name item
		 */
		FORCEINLINE std::wstring GetName() const
		{
			return ANSI_TO_TCHAR( name.c_str() );
		}

		/**
		 * Set callback
		 *
		 * @param[in] InCallback Function called when item triggered
		 */
		FORCEINLINE void SetCallback( FCallbackFunction InCallback )
		{
			callback = InCallback;
		}

		/**
		 * Get callback
		 * @return Return callback for triggered item
		 */
		FORCEINLINE FCallbackFunction GetCallback() const
		{
			return callback;
		}

	private:
		FCallbackFunction		callback;	/**< Function called when item triggered */
		std::string				name;		/**< Name item */
	};

	/**
	 * Class of menu in bar
	 */
	class FMenu : public FRefCounted
	{
	public:
		/**
		 * Constructor
		 */
		FMenu();

		/**
		 * Constructor
		 * 
		 * @param[in] InName Name menu
		 */
		FMenu( const tchar* InName );

		/**
		 * Add menu item
		 * 
		 * @param[in] InItem Pointer to item
		 */
		FORCEINLINE void AddItem( FMenuItem* InItem ) 
		{
			check( InItem );
			menuItems.push_back( InItem );
		}

		/**
		 * Remove menu item
		 * 
		 * @param[in] InName Name item
		 */
		FORCEINLINE void RemoveItem( const tchar* InName )
		{
			for ( uint32 index = 0, count = ( uint32 )menuItems.size(); index < count; ++index )
			{
				FMenuItem*		menuItem = menuItems[ index ];
				if ( menuItem->GetName() == InName )
				{
					menuItems.erase( menuItems.begin() + index );
					return;
				}
			}
		}

		/**
		 * Remove all menus
		 */
		FORCEINLINE void RemoveAllItems()
		{
			menuItems.clear();
		}

		/**
		 * Find menu item
		 * 
		 * @param[in] InName Name item
		 * @return Return pointer to menu item. If not found return nullptr
		 */
		TRefCountPtr< FMenuItem > FindItem( const tchar* InName ) const;

		/**
		 * Update menu items
		 */
		void Tick();

		/**
		 * Set name
		 * 
		 * @param[in] InName Name of menu
		 */
		FORCEINLINE void SetName( const tchar* InName )
		{
			name = TCHAR_TO_ANSI( InName );
		}

		/**
		 * Get name
		 * 
		 * @return Return name of menu bar
		 */
		FORCEINLINE std::wstring GetName() const
		{
			return ANSI_TO_TCHAR( name.c_str() );
		}

	private:
		std::string											name;			/**< Name of menu */
		std::vector< TRefCountPtr< FMenuItem > >			menuItems;		/**< Array of items in menu bar */
	};

	/**
	 * Add menu
	 *
	 * @param[in] InMenu Pointer to menu
	 */
	FORCEINLINE void AddMenu( FMenu* InMenu )
	{
		check( InMenu );
		menus.push_back( InMenu );
	}

	/**
	 * Remove menu
	 *
	 * @param[in] InName Name menu
	 */
	FORCEINLINE void RemoveMenu( const tchar* InName )
	{
		for ( uint32 index = 0, count = ( uint32 )menus.size(); index < count; ++index )
		{
			FMenu*			menu = menus[ index ];
			if ( menu->GetName() == InName )
			{
				menus.erase( menus.begin() + index );
				return;
			}
		}
	}

	/**
	 * Remove all menus
	 */
	FORCEINLINE void RemoveAllMenus()
	{
		menus.clear();
	}

	/**
	 * Find menu
	 *
	 * @param[in] InName Name menu
	 */
	TRefCountPtr< FMenu > FindMenu( const tchar* InName ) const;

	/**
	 * Update logic of main menu bar
	 */
	virtual void Tick() override;

private:
	std::vector< TRefCountPtr< FMenu > >			menus;			/**< Map of menu bars */
};

#endif // MAINMENUBAR_H