/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEWIDGET_H
#define BASEWIDGET_H

#include "Misc/RefCounted.h"

/**
 * @ingroup WorldEd
 * Base class of all widgets in world editor
 */
class WBaseWidget : public FRefCounted
{
public:
	/**
	 * Constructor
	 */
	WBaseWidget() :
		isInitialized( false )
	{}

	/**
	 * Destructor
	 */
	virtual ~WBaseWidget() 
	{}

	/**
	 * Initialize widget
	 */
	virtual void Init() 
	{
		isInitialized = true;
	}

	/**
	 * Update logic of widget
	 */
	virtual void Tick() 
	{}

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) {}

	/**
	 * Is initialized widget
	 * @return Return true if widget is initialized, else false
	 */
	FORCEINLINE bool IsInitialized() const
	{
		return isInitialized;
	}

private:
	bool			isInitialized;			/**< Is widget initialized */
};

#endif