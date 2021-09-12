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
	 * Destructor
	 */
	virtual ~WBaseWidget() {}

	/**
	 * Initialize widget
	 */
	virtual void Init() 
	{}

	/**
	 * Update logic of widget
	 */
	virtual void Tick() 
	{}

	/**
	 * Shutdown widget
	 */
	virtual void Shutdown() 
	{}

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) {}
};

#endif