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
#include <string>

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
	 * Enumeration root directories of contents
	 */
	enum ERootDir
	{
		RD_Game,		/**< Root of game content */
		RD_Engine,		/**< Root of engine content */
		RD_Max			/**< Count of root directories */
	};

	/**
	 * Constructor
	 * 
	 * @param InRootDir		Root directory
	 * @param InParent		Parent of widget
	 */
	WeContentBrowserWidget( ERootDir InRootDir = RD_Game, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeContentBrowserWidget();

	/**
	 * Set root directory
	 * @param InRootDir		Root directory
	 */
	void SetRootDir( ERootDir InRootDir );

	/**
	 * Show asset in content browser
	 * @param InAssetReference		Asset reference
	 */
	void ShowAsset( const std::wstring& InAssetReference );

	/**
	 * Get selected asset reference
	 * @return Return selected asset reference
	 */
	FORCEINLINE const std::wstring& GetSelectedAssetReference() const
	{
		return selectedAssetReference;
	}

private slots:
	//
	// Events
	//
	void on_treeView_contentBrowser_customContextMenuRequested( QPoint InPoint );
	void on_listView_packageBrowser_Import();
	void on_listView_packageBrowser_Reimport();
	void on_listView_packageBrowser_ReimportWithNewFile();
	void on_listView_packageBrowser_CreateMaterial();
	void on_listView_packageBrowser_DeleteAsset();
	void on_listView_packageBrowser_RenameAsset();
	void on_listView_packageBrowser_customContextMenuRequested( QPoint InPoint );
	void on_treeView_contentBrowser_contextMenu_delete();
	void on_treeView_contentBrowser_contextMenu_rename();
	void on_treeView_contentBrowser_contextMenu_createFolder();
	void on_treeView_contentBrowser_contextMenu_createPackage();
	void on_treeView_contentBrowser_contextMenu_SavePackage();
	void on_treeView_contentBrowser_contextMenu_OpenPackage();
	void on_treeView_contentBrowser_contextMenu_UnloadPackage();
	void OnContentBrowserClickedInEmptySpace();
	void OnContentBrowserSelectedPackage( FPackage* InPackage );
	void OnContentBrowserContextMenuShowInExplorer();
	void on_comboBox_rootDir_currentIndexChanged( int InIndex );
	void on_listView_packageBrowser_doubleClicked( QModelIndex InModelIndex );
	void OnPackageBrowserChangedAsset( const TSharedPtr<FAsset>& InAsset );
	void on_listView_packageBrowser_CopyReferenceToAsset();
	void OnListViewPackageBrowserSelectedAsset( const std::wstring& InAssetReference );
	void on_listView_packageBrowser_ReloadAsset();
	void on_treeView_contentBrowser_contextMenu_ReloadPackage();

private:
	Ui::WeContentBrowserWidget*	ui;							/**< Qt UI */
	std::wstring				selectedAssetReference;		/**< Selected asset */
};

#endif // !CONTENTBROWSERWIDGET_H