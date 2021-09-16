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
#include <list>

#include "Misc/WorldEdTypes.h"
#include "Math/Vector2D.h"
#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"
#include "System/Delegate.h"

DECLARE_DELEGATE( FOnWindowOpened, class WBaseWindow* )
DECLARE_DELEGATE( FOnWindowClosed, class WBaseWindow* )

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
		onWindowOpened.Broadcast( this );
	}

	/**
	 * Close window
	 */
	FORCEINLINE void Close()
	{
		isOpen = false;
		onWindowClosed.Broadcast( this );
	}

	/**
	 * Add widget
	 * 
	 * @param[in] InWidget Pointer to widget
	 */
	FORCEINLINE void AddWidget( FWBaseWidgetParamRef InWidget )
	{
		widgets.push_back( InWidget );
	}

	/**
	 * Remove widget
	 * 
	 * @param[in] InWidget Pointer to widget
	 */
	FORCEINLINE void RemoveWidget( FWBaseWidgetParamRef InWidget )
	{
		for ( auto it = widgets.begin(), itEnd = widgets.end(); it != itEnd; ++it )
		{
			if ( ( *it ) == InWidget )
			{
				widgets.erase( it );
				return;
			}
		}
	}

	/**
	 * Remove all widgets
	 */
	FORCEINLINE void RemoveAllWidgets()
	{
		widgets.clear();
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
	 * Get ANSI title window
	 * @return Return title window in ANSI format
	 */
	FORCEINLINE std::string GetATitle() const
	{
		return title;
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
	 * Set size window
	 * @param[in] InSize Window size
	 */
	FORCEINLINE void SetSize( const FVector2D& InSize )
	{
		size = InSize;
	}

	/**
	 * Get flags window
	 * @return Return flags window
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * Get size window
	 * @return Return size window
	 */
	FORCEINLINE const FVector2D& GetSize() const
	{
		return size;
	}

	/**
	 * Get delegate to event of open window
	 * @return Return reference to delegate event of open window
	 */
	FORCEINLINE FOnWindowOpened& OnWindowOpened()
	{
		return onWindowOpened;
	}

	/**
	 * Get delegate to event of close window
	 * @return Return reference to delegate event of close window
	 */
	FORCEINLINE FOnWindowClosed& OnWindowClosed()
	{
		return onWindowClosed;
	}

private:
	bool								isOpen;				/**< Is opened window */
	std::string							title;				/**< Title window */
	uint32								flags;				/**< Flags window for ImGUI */
	std::list< FWBaseWidgetRef >		widgets;			/**< Array of widgets in window */
	FVector2D							size;				/**< Size window */
	FOnWindowOpened						onWindowOpened;		/**< Event of open window */
	FOnWindowClosed						onWindowClosed;		/**< Event of close window */
};

#endif // !WORLDED_BASEWINDOW_H