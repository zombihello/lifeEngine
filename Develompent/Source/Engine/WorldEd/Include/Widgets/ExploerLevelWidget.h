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
 * @brief Widget of showing scene graph
 */
class WeExploerLevelWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * @brief Constructor
	 * @param InParent		Parent
	 */
	WeExploerLevelWidget( QWidget* InParent = nullptr );

	/**
	 * @brief Destructor
	 */
	virtual ~WeExploerLevelWidget();

private slots:
	//
	// Events
	//
	void on_listView_level_customContextMenuRequested( QPoint InPoint );
	void OnContextMenuActorSpawn();
	void OnContextMenuActorRename();
	void OnContextMenuActorDuplicate();
	void OnContextMenuActorDelete();
	void OnContextMenuActorProperties();

private:
	Ui::WeExploerLevelWidget*		ui;			/**< Qt UI */
};

#endif // !EXPLOERLEVELWIDGET_H