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
};

#endif // MAINWINDOW_H