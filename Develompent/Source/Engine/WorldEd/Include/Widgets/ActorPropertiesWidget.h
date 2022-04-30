/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORPROPERTIESWIDGET_H
#define ACTORPROPERTIESWIDGET_H

#include <QWidget>

namespace Ui
{
	class WeActorPropertiesWidget;
}

/**
 * @ingroup WorldEd
 * Widget for show properties and setting of actors
 */
class WeActorPropertiesWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	WeActorPropertiesWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeActorPropertiesWidget();

private:
	Ui::WeActorPropertiesWidget*				ui;			/**< Qt UI */

};

#endif // !LOGWIDGET_H