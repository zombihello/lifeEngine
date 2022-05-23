/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATERIALEDITORWINDOW_H
#define MATERIALEDITORWINDOW_H

#include <QWidget>
#include "Render/Material.h"
#include "WorldEd.h"

namespace Ui
{
	class WeMaterialEditorWindow;
}

/**
 * @ingroup WorldEd
 * Class window of material editor
 */
class WeMaterialEditorWindow : public QWidget
{
	Q_OBJECT

public:
	/**
	 * Constructor
	 * 
	 * @param InMaterial	Material to edit
	 * @param InParent		Parent widget
	 */
	WeMaterialEditorWindow( const TSharedPtr<FMaterial>& InMaterial, QWidget* InParent = nullptr );

	/**
	 * Destructor
	 */
	virtual ~WeMaterialEditorWindow();

signals:
	/**
	 * Event when material is changed
	 * 
	 * @param InAsset		Changed asset
	 */
	void OnChangedAsset( const TSharedPtr<FAsset>& InAsset );

private slots:
	//
	// Events
	//
	void OnCheckBoxIsTwoSidedToggled( bool InValue );
	void OnCheckBoxIsWireframeToggled( bool InValue );
	void OnCheckBoxStaticMeshToggled( bool InValue );
	void OnCheckBoxSpriteToggled( bool InValue );
	void OnSelectedAssetDiffuse( const std::wstring& InNewAssetReference );

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
	 * Called event when asset try delete
	 *
	 * @param InAssets	Array of assets to delete
	 * @param OutResult Result, we can is delete this assets?
	 */
	void OnAssetsCanDelete( const std::vector< TSharedPtr<class FAsset> >& InAssets, struct FCanDeleteAssetResult& OutResult );

	/**
	 * Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector< TSharedPtr<class FAsset> >& InAssets );

	bool													bInit;						/**< Is inited window */
	Ui::WeMaterialEditorWindow*								ui;							/**< Qt UI */
	TSharedPtr<FMaterial>									material;					/**< Editable asset */
	class QCheckBox*										checkBox_isTwoSided;		/**< Check box of is two sided */
	class QCheckBox*										checkBox_isWireframe;		/**< Check box of is wireframe */
	class QCheckBox*										checkBox_staticMesh;		/**< Check box of static mesh */
	class QCheckBox*										checkBox_sprite;			/**< Check box of sprite */
	class WeSelectAssetWidget*								selectAsset_diffuse;		/**< Widget for select asset (diffuse texture) */
	class FMaterialPreviewViewportClient*					viewportClient;				/**< Viewport client */
	FEditorDelegates::FOnAssetsCanDelete::FDelegateType*	assetsCanDeleteHandle;		/**< Handle delegate of assets can delete */
	FEditorDelegates::FOnAssetsReloaded::FDelegateType*		assetsReloadedHandle;		/**< Handle delegate of reloaded assets */
};

#endif // MATERIALEDITORWINDOW_H