/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EXPLOERLEVELWIDGET_H
#define EXPLOERLEVELWIDGET_H

#include <QWidget>

namespace Ui
{
	class WeExploerLevelWidget;
}

/**
 * @ingroup WorldEd
 * Widget of showing scene graph
 */
class WeExploerLevelWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	WeExploerLevelWidget( QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeExploerLevelWidget();

private:
	Ui::WeExploerLevelWidget*				ui;			/**< Qt UI */

};

#endif // !EXPLOERLEVELWIDGET_H