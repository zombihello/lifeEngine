/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERLEVEL_H
#define EXPLOERLEVEL_H

#include <qlistview.h>
#include <qproxystyle.h>
#include <qbasictimer.h>

#include "System/ExploerLevelModel.h"

/**
 * @ingroup WorldEd
 * Class of exploer level
 */
class WeExploerLevel : public QListView
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

private slots:
	//
	// Events
	//
	void OnSelectionChanged( const QItemSelection& InSelected, const QItemSelection& InDeselected );

protected:
	/**
	 * Event of press mouse
	 * 
	 * @param InEvent Mouse press event
	 */
	virtual void mousePressEvent( QMouseEvent* InEvent ) override;

	/**
	 * Event called when actors is selected
	 * @param InActors		Array of actors
	 */
	void OnSelectedActors( const std::vector<ActorRef_t>& InActors );

	/**
	 * Event called when actors is unselected
	 * @param InActors		Array of actors
	 */
	void OnUnselectedActors( const std::vector<ActorRef_t>& InActors );

private:
	bool														bIgnoreEventSelectionChanged;	/**< Is need ignore event of selection changed */
	WeExploerLevelModel*										exploerLevelModel;				/**< Exploer level model */
	WeStyle*													style;							/**< Custom style */
	QPoint														dragStartPosition;				/**< Drag start position */
	SEditorDelegates::COnActorsSelected::DelegateType_t*		actorsSelectedDelegate;			/**< Delegate of actors selected */
	SEditorDelegates::COnActorsUnselected::DelegateType_t*		actorsUnselectedDelegate;		/**< Delegate of actors unselected */
};

#endif // !EXPLOERLEVEL_H