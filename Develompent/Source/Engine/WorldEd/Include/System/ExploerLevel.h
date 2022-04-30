/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERLEVEL_H
#define EXPLOERLEVEL_H

#include <qtreeview.h>
#include <qproxystyle.h>
#include <qbasictimer.h>

/**
 * @ingroup WorldEd
 * Class of exploer level
 */
class WeExploerLevel : public QTreeView
{
	Q_OBJECT

public:
	/**
	 * Class of style this exploer level
	 */
	class WeStyle : public QProxyStyle
	{
	public:
		/**
		 * Draw primitive
		 */
		virtual void drawPrimitive( PrimitiveElement InElement, const QStyleOption* InOption, QPainter* InPainter, const QWidget* InWidget ) const override;
	};

	/**
	 * Constructor
	 */
	WeExploerLevel( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeExploerLevel();

protected:
	/**
	 * Event of press mouse
	 * 
	 * @param InEvent Mouse press event
	 */
	virtual void mousePressEvent( QMouseEvent* InEvent ) override;

	/**
	 * Event of drop
	 * 
	 * @param InEvent Drop event
	 */
	virtual void dropEvent( QDropEvent* InEvent ) override;

private:
	WeStyle*					style;					/**< Custom style */
	QPoint						dragStartPosition;		/** Drag start position */
};

#endif // !EXPLOERLEVEL_H