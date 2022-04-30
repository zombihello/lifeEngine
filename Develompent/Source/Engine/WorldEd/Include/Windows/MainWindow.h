/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <DockManager.h>

#include "Render/Viewport.h"
#include "Core.h"

namespace Ui
{
	class MainWindow;
}

/**
 * @ingroup WorldEd
 * Class main window of WorldEd
 */
class WeMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * param[in] InParent Parent widget
	 */
	WeMainWindow( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeMainWindow();

	/**
	 * Event of key press
	 * 
	 * @param InEvent Event of key press
	 */
	virtual void keyPressEvent( QKeyEvent* InEvent ) override;

	/**
	 * Event of key release
	 *
	 * @param InEvent Event of key release
	 */
	virtual void keyReleaseEvent( QKeyEvent* InEvent ) override;

	/**
	 * Event of mouse press
	 * 
	 * @param InEvent Event of mouse press
	 */
	virtual void mousePressEvent( QMouseEvent* InEvent ) override;

	/**
	 * Event of mouse release
	 *
	 * @param InEvent Event of mouse release
	 */
	virtual void mouseReleaseEvent( QMouseEvent* InEvent ) override;

	/**
	 * Event of mouse move
	 * 
	 * @param InEvent Event of mouse move
	 */
	virtual void mouseMoveEvent( QMouseEvent* InEvent ) override;

	/**
	 * Get log widget
	 * @return Return log widget. If not created returning nullptr
	 */
	FORCEINLINE class WeLogWidget* GetLogWidget() const
	{
		return logWidget;
	}

private slots:
	/**
	 * Slot on tick LE
	 */
	void OnTickLE();

	//
	// Events
	//
	void on_actionAbout_Qt_triggered();
	void on_actionAbout_triggered();
	void On_PerspectiveDockWidget_VisibilityChanged( bool InVisible );
	void On_OrthoXYDockWidget_VisibilityChanged( bool InVisible );
	void On_OrthoXZDockWidget_VisibilityChanged( bool InVisible );
	void On_OrthoYZDockWidget_VisibilityChanged( bool InVisible );

private:
	/**
	 * Init UI
	 */
	void InitUI();

	/**
	 * Create dock widget
	 * 
	 * @param InArea		Area to dock new widget
	 * @param InTitle		Title
	 * @param InContent		Widget in dock window
	 * @param InParent		Parent of dock widget
	 * @param OutDockWidget	If not NULL return pointer to created dock widget
	 */
	FORCEINLINE ads::CDockAreaWidget* CreateDockWidget( ads::DockWidgetArea InArea, const QString& InTitle, QWidget* InContent, ads::CDockAreaWidget* InParent = nullptr, ads::CDockWidget** OutDockWidget = nullptr )
	{
		check( InContent && dockManager );

		ads::CDockWidget*		dockWidget = new ads::CDockWidget( InTitle );
		dockWidget->setWidget( InContent );
		if ( OutDockWidget )
		{
			*OutDockWidget = dockWidget;
		}

		return dockManager->addDockWidget( InArea, dockWidget, InParent );
	}

	Ui::MainWindow*					ui;								/**< Qt UI */
	QTimer							timerTick;						/**< Timer for tick engine */
	QPoint							mousePosition;					/**< Mouse position */
	ads::CDockManager*				dockManager;					/**< Dock manager */
	class WeLogWidget*				logWidget;						/**< Log widget */
	class WeViewportWidget*			viewportWidgets[ LVT_Max ];		/**< Array of widget viewports */
};

#endif // MAINWINDOW_H