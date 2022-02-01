/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>
#include "Logger/LoggerMacros.h"

namespace Ui
{
	class WeLogWidget;
}

/**
 * @ingroup WorldEd
 * Widget of print logs on screen
 */
class WeLogWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	WeLogWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeLogWidget();

	/**
	 * Print message
	 *
	 * @param InLogType Log type
	 * @param InMessage Message of log
	 */
	virtual void Print( ELogType InLogType, const tchar* InMessage );

private:
	Ui::WeLogWidget*				ui;			/**< Qt UI */

};

#endif // !LOGWIDGET_H