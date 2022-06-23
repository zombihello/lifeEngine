/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ACTORCLASSESWIDGET_H
#define ACTORCLASSESWIDGET_H

#include <QWidget>

namespace Ui
{
	class WeActorClassesWidget;
}

/**
 * @ingroup WorldEd
 * Widget for show and selecting current actor class for spawn in world
 */
class WeActorClassesWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * @param InParent		Parent
	 */
	WeActorClassesWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeActorClassesWidget();

private:
	/**
	 * Init UI
	 */
	void InitUI();
	
	Ui::WeActorClassesWidget*		ui;						/**< Qt UI */
	class WeExploerClassesModel*	exploerClassesModel;	/**< Exploer classes model */
};

#endif // !ACTORCLASSESWIDGET_H