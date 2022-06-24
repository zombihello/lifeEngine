/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVELVIEWPORTWIDGET_H
#define LEVELVIEWPORTWIDGET_H

#include "Render/EditorLevelViewportClient.h"
#include "Widgets/ViewportWidget.h"

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

private slots:
	//
	// Events
	//
	void OnCustomContextMenuRequested( const QPoint& InPoint );
	void OnActorAdd();
	void OnAssetAdd();
};

#endif // !LEVELVIEWPORTWIDGET_H