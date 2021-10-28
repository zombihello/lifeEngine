/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTWIDGET_H
#define VIEWPORTWIDGET_H

#include <QWidget>

#include "Render/Viewport.h"

/**
 * @ingroup WorldEd
 * Widget viewport for render scene and tick engine
 */
class WeViewportWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 *
	 * @param[in] InParent Parent widget
	 */
	WeViewportWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeViewportWidget();

	/**
	 * Get engine viewport
	 * @return Return engine viewport
	 */
	FORCEINLINE const FViewport& GetViewport() const
	{
		return viewport;
	}

private:
	/**
	 * Event called when widget showed
	 * 
	 * @param[in] InEvent Event of show
	 */
	virtual void showEvent( QShowEvent* InEvent ) override;

	/**
	 * Return Qt paint engine
	 * @return Return Qt paint engine
	 */
	virtual QPaintEngine* paintEngine() const override;

	/**
	 * Event called when paint widget
	 * 
	 * @param[in] InEvent Event of paint widget
	 */
	virtual void paintEvent( QPaintEvent* InEvent ) override;

	/**
	 * Event called when resize widget
	 * 
	 * @param[in] InEvent Event of resize widget
	 */
	virtual void resizeEvent( QResizeEvent* InEvent ) override;

	bool			isInTick;			/**< Is added viewport to editor engine */
	FViewport		viewport;			/**< Viewport of render */
};

#endif // !VIEWPORTWIDGET_H