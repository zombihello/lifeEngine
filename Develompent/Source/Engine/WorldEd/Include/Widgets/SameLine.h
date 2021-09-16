/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_SAMELINE_H
#define WORLDED_SAMELINE_H

#include <list>

#include "Misc/WorldEdTypes.h"
#include "Widgets/BaseWidget.h"

/**
 * @ingroup WorldEd
 * Class for contain widgets in same line
 */
class WSameLine : public WBaseWidget
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InText Text in widget
	 */
	WSameLine();

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;

	/**
	 * Add widget in line
	 * 
	 * @param[in] InWidget Pointer to widget
	 */
	FORCEINLINE void AddWidget( FWBaseWidgetParamRef InWidget )
	{
		widgets.push_back( InWidget );
	}

	/**
	 * Remove widgets from line
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
	 * Remove all widgets from line
	 */
	FORCEINLINE void RemoveAllWidgets()
	{
		widgets.clear();
	}

private:
	std::list< FWBaseWidgetRef >		widgets;		/**< Array of widgets */
};

#endif // !WORLDED_TEXT_H