/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_BASEWINDOW_H
#define WORLDED_BASEWINDOW_H

#include <string>
#include <unordered_map>

#include "Misc/WorldEdTypes.h"
#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"

/**
 * @ingroup WorldEd
 * Base class for work with window in world editor
 */
class WBaseWindow : public WBaseWidget
{
public:
	/**
	 * Constructor
	 */
	WBaseWindow();

	/**
	 * Constructor
	 * 
	 * @param[in] InTitle Title window
	 * @param[in] InFlags Flags window of ImGUI
	 */
	WBaseWindow( const tchar* InTitle = TEXT( "" ), uint32 InFlags = 0 );

	/**
	 * Destructor
	 */
	virtual ~WBaseWindow() {}

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;

	/**
	 * Open window
	 */
	FORCEINLINE void Open()
	{
		isOpen = true;
	}

	/**
	 * Close window
	 */
	FORCEINLINE void Close()
	{
		isOpen = false;
	}

	/**
	 * Add widget
	 * 
	 * @param[in] InName Name widget
	 * @param[in] InWidget Pointer to widget
	 */
	FORCEINLINE void AddWidget( const tchar* InName, FWBaseWidgetParamRef InWidget )
	{
		widgets[ InName ] = InWidget;
	}

	/**
	 * Remove widget
	 * 
	 * @param[in] InName Name widget
	 */
	FORCEINLINE void RemoveWidget( const tchar* InName )
	{
		widgets.erase( InName );
	}

	/**
	 * Remove all widgets
	 */
	FORCEINLINE void RemoveAllWidget()
	{
		widgets.clear();
	}

	/**
	 * Find widget
	 * 
	 * @param[in] InName Name widget
	 * @return Return pointer to widget. If not found return nullptr
	 */
	FORCEINLINE FWBaseWidgetRef FindWidget( const tchar* InName ) const
	{
		auto	it = widgets.find( InName );
		if ( it == widgets.end() )
		{
			return nullptr;
		}

		return it->second;
	}

	/**
	 * Set title window
	 * 
	 * @param[in] InTitle Title window
	 */
	FORCEINLINE void SetTitle( const tchar* InTitle )
	{
		title = TCHAR_TO_ANSI( InTitle );
	}

	/**
	 * Get title window
	 * @return Return title window
	 */
	FORCEINLINE std::wstring GetTitle() const
	{
		return ANSI_TO_TCHAR( title.c_str() );
	}

	/**
	 * Get is opened window
	 * @return Return true if window is open, else return false
	 */
	FORCEINLINE bool IsOpen() const
	{
		return isOpen;
	}

	/**
	 * Set flags window
	 * @param[in] InFlags Flags window of ImGUI
	 */
	FORCEINLINE void SetFlags( uint32 InFlags )
	{
		flags = InFlags;
	}

	/**
	 * Get flags window
	 * @return Return flags window
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

private:
	bool														isOpen;			/**< Is opened window */
	std::string													title;			/**< Title window */
	uint32														flags;			/**< Flags window for ImGUI */
	std::unordered_map< std::wstring, FWBaseWidgetRef >			widgets;		/**< Map of widgets in window */
};

#endif // !WORLDED_BASEWINDOW_H