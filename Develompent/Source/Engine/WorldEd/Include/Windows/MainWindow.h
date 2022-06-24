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
	 * Event of close window
	 * 
	 * @param InEvent Event of close event
	 */
	virtual void closeEvent( QCloseEvent* InEvent ) override;

	/**
	 * Create dock widget
	 *
	 * @param InArea					Area to dock new widget
	 * @param InTitle					Title
	 * @param InContent					Widget in dock window
	 * @param InParent					Parent of dock widget
	 * @param InIsDeleteAfterClose		Is need delete widget after close
	 * @param OutDockWidget				If not NULL return pointer to created dock widget
	 * @return Return pointer to dock area widget
	 */
	FORCEINLINE ads::CDockAreaWidget* CreateDockWidget( ads::DockWidgetArea InArea, const QString& InTitle, QWidget* InContent, ads::CDockAreaWidget* InParent = nullptr, bool InIsDeleteAfterClose = false, ads::CDockWidget** OutDockWidget = nullptr )
	{
		check( InContent && dockManager );

		ads::CDockWidget* dockWidget = new ads::CDockWidget( InTitle );
		dockWidget->setWidget( InContent );
		if ( OutDockWidget )
		{
			*OutDockWidget = dockWidget;
		}

		dockWidget->setFeature( ads::CDockWidget::DockWidgetDeleteOnClose, InIsDeleteAfterClose );
		dockWidget->setMinimumSize( InContent->minimumSize() );
		dockWidget->setMaximumSize( InContent->maximumSize() );

		return dockManager->addDockWidget( InArea, dockWidget, InParent );
	}

	/**
	 * Create floating dock widget
	 * 
	 * @param InTitle					Title
	 * @param InContent					Widget in dock window
	 * @param InIsDeleteAfterClose		Is need delete widget after close
	 * @param OutDockWidget				If not NULL return pointer to created dock widget
	 * @return Return floating dock container
	 */
	FORCEINLINE ads::CFloatingDockContainer* CreateFloatingDockWidget( const QString& InTitle, QWidget* InContent, bool InIsDeleteAfterClose = false, ads::CDockWidget** OutDockWidget = nullptr )
	{
		check( InContent && dockManager );

		ads::CDockWidget* dockWidget = new ads::CDockWidget( InTitle );
		dockWidget->setWidget( InContent );
		if ( OutDockWidget )
		{
			*OutDockWidget = dockWidget;
		}

		dockWidget->setFeature( ads::CDockWidget::DockWidgetDeleteOnClose, InIsDeleteAfterClose );

		ads::CFloatingDockContainer*	floatingDockContainer = dockManager->addDockWidgetFloating( dockWidget );
		floatingDockContainer->setMinimumSize( InContent->minimumSize() );
		floatingDockContainer->setMaximumSize( InContent->maximumSize() );
		return floatingDockContainer;
	}

	/**
	 * Get log widget
	 * @return Return log widget. If not created returning nullptr
	 */
	FORCEINLINE class WeLogWidget* GetLogWidget() const
	{
		return logWidget;
	}

	/**
	 * Get dock manager
	 * @return Return dock manager
	 */
	FORCEINLINE ads::CDockManager* GetDockManager() const
	{
		return dockManager;
	}

	/**
	 * Get content browser widget
	 * @return Return content browser widget
	 */
	FORCEINLINE class WeContentBrowserWidget* GetContentBrowserWidget() const
	{
		return contentBrowserWidget;
	}

	/**
	 * Get actor classes widget
	 * @return Return actor classes widget
	 */
	FORCEINLINE class WeActorClassesWidget* GetActorClassesWidget() const
	{
		return actorClassesWidget;
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

	Ui::MainWindow*					ui;								/**< Qt UI */
	QTimer							timerTick;						/**< Timer for tick engine */
	QPoint							mousePosition;					/**< Mouse position */
	ads::CDockManager*				dockManager;					/**< Dock manager */
	class WeLogWidget*				logWidget;						/**< Log widget */
	class WeLevelViewportWidget*	viewportWidgets[ LVT_Max ];		/**< Array of widget viewports */
	class WeContentBrowserWidget*	contentBrowserWidget;			/**< Content browser widget */
	class WeActorClassesWidget*		actorClassesWidget;				/**< Actor classes widget */
};

#endif // MAINWINDOW_H