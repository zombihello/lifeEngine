/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONTENTBROWSERWIDGET_H
#define CONTENTBROWSERWIDGET_H

#include <QWidget>
#include "Core.h"

namespace Ui
{
	class WeContentBrowserWidget;
}

/**
 * @ingroup WorldEd
 * Widget of showing contents in file system
 */
class WeContentBrowserWidget : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 */
	WeContentBrowserWidget( const tchar* InRootDir, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeContentBrowserWidget();

private slots:
	/**
	 * File explorer context menu open event
	 * 
	 * @param InPoint Point of click in content browser
	 */
	void on_treeView_contentBrowser_customContextMenuRequested( QPoint InPoint );

	/**
	 * File Explorer context menu selection event "Delete"
	 */
	void on_treeView_contentBrowser_contextMenu_delete();

	/**
	 * File Explorer context menu selection event "Rename"
	 */
	void on_treeView_contentBrowser_contextMenu_rename();

	/**
	 * File Explorer context menu selection event "Create Folder"
	 */
	void on_treeView_contentBrowser_contextMenu_createFolder();

	/**
	 * File explorer context menu selection event "Create Material"
	 */
	void on_treeView_contentBrowser_contextMenu_createMaterial();

	/**
	 * Event open package
	 * @param InPackage Package
	 */
	void on_treeView_contentBrowser_OnOpenPackage( class FPackage* InPackage );

private:
	Ui::WeContentBrowserWidget*				ui;			/**< Qt UI */

};

#endif // !CONTENTBROWSERWIDGET_H