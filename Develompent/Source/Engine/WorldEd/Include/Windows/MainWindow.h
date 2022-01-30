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

private slots:
	/**
	 * Slot on tick LE
	 */
	void OnTickLE();

	/**
	 * Trigger 'About Qt'
	 */
	void on_actionAbout_Qt_triggered();

	/**
	 * Trigger 'About WorldEd'
	 */
	void on_actionAbout_triggered();

private:
	Ui::MainWindow*					ui;					/**< Qt UI */
	QTimer							timerTick;			/**< Timer for tick engine */
	QPoint							mousePosition;		/**< Mouse position */
};

#endif // MAINWINDOW_H