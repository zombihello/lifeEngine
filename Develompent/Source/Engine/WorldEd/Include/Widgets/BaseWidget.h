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
#include "Math/Vector2D.h"

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
	WBaseWidget();

	/**
	 * Destructor
	 */
	virtual ~WBaseWidget();

	/**
	 * Initialize widget
	 */
	virtual void Init();

	/**
	 * Update logic of widget
	 */
	virtual void Tick();

	/**
	 * Process event
	 *
	 * @param[in] InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent );

	/**
	 * Set position
	 * 
	 * @param[in] InPosition Position widget
	 */
	FORCEINLINE void SetPosition( const FVector2D& InPosition )
	{
		position = InPosition;
	}

	/**
	 * Enable or disable widget
	 * 
	 * @param[in] IsEnabled Is enabled widget
	 */
	FORCEINLINE void SetEnable( bool IsEnabled )
	{
		isEnabled = IsEnabled;
	}

	/**
	 * Is initialized widget
	 * @return Return true if widget is initialized, else false
	 */
	FORCEINLINE bool IsInitialized() const
	{
		return isInitialized;
	}

	/**
	 * Get position
	 * @return Return position of widget
	 */
	FORCEINLINE const FVector2D& GetPosition() const
	{
		return position;
	}

	/**
	 * Is enabled widget
	 * @return Return true if widget is enabled, else return false
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return isEnabled;
	}

private:
	bool			isInitialized;			/**< Is widget initialized */
	bool			isEnabled;				/**< Is enabled widget */
	FVector2D		position;				/**< Position of widget */
};

#endif