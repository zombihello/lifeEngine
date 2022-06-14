/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHEDITORWINDOW_H
#define STATICMESHEDITORWINDOW_H

#include <vector>
#include <QWidget>

#include "Render/StaticMesh.h"
#include "WorldEd.h"

namespace Ui
{
	class WeStaticMeshEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of static mesh editor
 */
class WeStaticMeshEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InStaticMesh	Static mesh to edit
	 * @param InParent		Parent widget
	 */
	WeStaticMeshEditorWindow( const TSharedPtr<CStaticMesh>& InStaticMesh, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeStaticMeshEditorWindow();

signals:
	/**
	 * Event when static mesh is changed
	 * 
	 * @param InAsset		Changed asset
	 */
	void OnChangedAsset( const TSharedPtr<CAsset>& InAsset );

private slots:
	//
	// Events
	//
	void OnSelectedAssetMaterial( uint32 InAssetSlot, const std::wstring& InNewAssetReference );
	void on_toolButton_sourceFile_clicked();
	void on_toolButton_sourceFileRemove_clicked();
	void on_actionReimport_triggered();

private:
	/**
	 * Init UI
	 */
	void InitUI();

	/**
	 * Update UI
	 */
	void UpdateUI();

	/**
	 * Called when source file changed
	 *
	 * @param InNewSourceFile		New path to source file
	 */
	void OnSourceFileChanged( QString InNewSourceFile );

	/**
	 * Crop source file text in UI
	 */
	void UICropSourceFileText();

	/**
	 * Event called when resize widget
	 *
	 * @param[in] InEvent Event of resize widget
	 */
	virtual void resizeEvent( QResizeEvent* InEvent ) override;

	/**
	 * Called event when asset try delete
	 *
	 * @param InAssets	Array of assets to delete
	 * @param OutResult Result, we can is delete this assets?
	 */
	void OnAssetsCanDelete( const std::vector< TSharedPtr<class CAsset> >& InAssets, struct SCanDeleteAssetResult& OutResult );

	/**
	 * Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector< TSharedPtr<class CAsset> >& InAssets );

	bool													bInit;							/**< Is inited window */
	Ui::WeStaticMeshEditorWindow*							ui;								/**< Qt UI */
	TSharedPtr<CStaticMesh>									staticMesh;						/**< Editable asset */
	class CStaticMeshPreviewViewportClient*					viewportClient;					/**< Viewport client */
	SEditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;			/**< Handle delegate of assets can delete */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;			/**< Handle delegate of reloaded assets */
	class QLabel*											label_sourceFileValue;			/**< Label for show path to source file */
	class QToolButton*										toolButton_sourceFile;			/**< Tool button for select new path to source file */
	class QToolButton*										toolButton_sourceFileRemove;	/**< Tool button for remove path to source file */
	std::vector<class WeSelectAssetWidget*>					selectAsset_materials;			/**< Array of selection asset components */
};

#endif // STATICMESHEDITORWINDOW_H