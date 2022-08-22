/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVELVIEWPORTWIDGET_H
#define LEVELVIEWPORTWIDGET_H

#include <QTimer>

#include "Render/EditorLevelViewportClient.h"
#include "Widgets/ViewportWidget.h"
#include "System/Gizmo.h"
#include "WorldEd.h"
#include "LEBuild.h"

/**
 * @ingroup WorldEd
 * Widget level viewport for render scene and tick engine
 */
class WeLevelViewportWidget : public WeViewportWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param InParent					Parent widget
	 * @param InViewportClient			Viewport client
	 * @param InDeleteViewportClient	Is need delete viewport client in destroy this widget
	 */
	WeLevelViewportWidget( QWidget* InParent = nullptr, CEditorLevelViewportClient* InViewportClient = nullptr, bool InDeleteViewportClient = false );

	/**
	 * Destructor
	 */
	virtual ~WeLevelViewportWidget();

#if ENABLE_HITPROXY
	/**
	 * Event of mouse press
	 *
	 * @param InEvent Event of mouse press
	 */
	virtual void mousePressEvent( QMouseEvent* InEvent ) override;

	/**
	 * Set enabled
	 * @param InIsEnabled		Is enabled viewport
	 */
	virtual void SetEnabled( bool InIsEnabled ) override;

	/**
	 * Event of mouse enter on widget
	 * 
	 * @param InEvent Event of mouse enter on widget
	 */
	virtual void enterEvent( QEvent* InEvent ) override;

	/**
	 * Event of mouse leave from widget
	 * 
	 * @param InEvent Event of mouse leave from widget
	 */
	virtual void leaveEvent( QEvent* InEvent ) override;
#endif // ENABLE_HITPROXY

private slots:
#if ENABLE_HITPROXY
	/**
	 * Slot on rendering hit proxy of gizmo
	 */
	void OnRenderHitProxyGizmo();
#endif // ENABLE_HITPROXY

	//
	// Events
	//
	void OnCustomContextMenuRequested( const QPoint& InPoint );
	void OnActorAdd();
	void OnAssetAdd();
	void OnTick();

private:
	QPoint		contextMenuCursorPosition;		/**< Cursor position when opened context menu */
	CGizmo		gizmo;							/**< Gizmo for rendering pivot */

#if ENABLE_HITPROXY
	/**
	 * Update timer for render hit proxy gizmo
	 * This function start and stop timer 'timerRenderHitProxyGizmo' in depending on the gizmo status
	 * 
	 * @param InNeedStart	Is need start timer. If gizmo is disabled ignored parameter and stop timer
	 */
	void UpdateTimerRenderHitProxyGizmo( bool InNeedStart );

	QTimer		timerRenderHitProxyGizmo;		/**< Timer for rendering hit proxy of gizmo */
#endif // ENABLE_HITPROXY

	/**
	 * Event called when actors selected/unselected
	 */
	void OnUpdateActorSelections( const std::vector<ActorRef_t>& InActors );

	QTimer														timerTick;						/**< Timer for tick logic */
	SEditorDelegates::COnActorsSelected::DelegateType_t*		actorsSelectedDelegate;			/**< Delegate of actors selected */
	SEditorDelegates::COnActorsUnselected::DelegateType_t*		actorsUnselectedDelegate;		/**< Delegate of actors unselected */
};

#endif // !LEVELVIEWPORTWIDGET_H