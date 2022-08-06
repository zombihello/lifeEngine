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
	WeMaterialEditorWindow( const TSharedPtr<CMaterial>& InMaterial, QWidget* InParent = nullptr );

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
	void OnChangedAsset( const TSharedPtr<CAsset>& InAsset );

private slots:
	//
	// Events
	//
	void OnCheckBoxOnlyEditorToggled( bool InValue );
	void OnCheckBoxIsTwoSidedToggled( bool InValue );
	void OnCheckBoxIsWireframeToggled( bool InValue );
	void OnCheckBoxStaticMeshToggled( bool InValue );
	void OnCheckBoxSpriteToggled( bool InValue );
	void OnSelectedAssetDiffuse( uint32 InAssetSlot, const std::wstring& InNewAssetReference );

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
	void OnAssetsCanDelete( const std::vector< TSharedPtr<class CAsset> >& InAssets, struct SCanDeleteAssetResult& OutResult );

	/**
	 * Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector< TSharedPtr<class CAsset> >& InAssets );

	bool													bInit;						/**< Is inited window */
	Ui::WeMaterialEditorWindow*								ui;							/**< Qt UI */
	TSharedPtr<CMaterial>									material;					/**< Editable asset */
	class QCheckBox*										checkBox_onlyEditor;		/**< Check box of only editor */
	class QCheckBox*										checkBox_isTwoSided;		/**< Check box of is two sided */
	class QCheckBox*										checkBox_isWireframe;		/**< Check box of is wireframe */
	class QCheckBox*										checkBox_staticMesh;		/**< Check box of static mesh */
	class QCheckBox*										checkBox_sprite;			/**< Check box of sprite */
	class WeSelectAssetWidget*								selectAsset_diffuse;		/**< Widget for select asset (diffuse texture) */
	class CMaterialPreviewViewportClient*					viewportClient;				/**< Viewport client */
	SEditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;		/**< Handle delegate of assets can delete */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*		assetsReloadedHandle;		/**< Handle delegate of reloaded assets */
};

#endif // MATERIALEDITORWINDOW_H