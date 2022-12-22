/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATERIALEDITORWINDOW_H
#define MATERIALEDITORWINDOW_H

#include <vector>

#include "ImGUI/ImGUIEngine.h"
#include "Render/Material.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/SelectAssetWidget.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Material editor window
 */
class CMaterialEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 * @param InMaterial	Material
	 */
	CMaterialEditorWindow( const TSharedPtr<CMaterial>& InMaterial );

	/**
	 * @brief Destructor
	 */
	~CMaterialEditorWindow();

	/**
	 * @brief Init
	 */
	virtual void Init() override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Select asset handle
	 */
	struct SSelectAssetHandle
	{
		CName								parameterName;		/**< Parameter name */
		TAssetHandle<CTexture2D>			asset;				/**< Asset */
		TSharedPtr<CSelectAssetWidget>		widget;				/**< Select asset widget */
	};

	/**
	 * @brief Update asset info
	 */
	void UpdateAssetInfo();

	/**
	 * @brief Event called when selected asset
	 * 
	 * @param InAssetSlot			Asset slot
	 * @param InNewAssetReference	New asset reference
	 */
	void OnSelectedAsset( uint32 InAssetSlot, const std::wstring& InNewAssetReference );

	/**
	 * @brief Called event when asset try delete
	 *
	 * @param InAssets	Array of assets to delete
	 * @param OutResult Result, we can is delete this assets?
	 */
	void OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, struct SCanDeleteAssetResult& OutResult );

	/**
	 * @brief Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector<TSharedPtr<CAsset>>& InAssets );

	/**
	 * @brief Event called when need open asset editor
	 *
	 * @param InAssetSlot			Asset slot
	 */
	void OnOpenAssetEditor( uint32 InAssetSlot );

	bool													bIsDefaultMaterial;		/**< Is default material */
	TSharedPtr<CMaterial>									material;				/**< Material */
	CViewportWidget											viewportWidget;			/**< Viewport widget */
	class CMaterialPreviewViewportClient*					viewportClient;			/**< Viewport client */
	std::vector<SSelectAssetHandle>							selectAssetWidgets;		/**< Array of select asset widgets */
	SEditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;	/**< Handle delegate of assets can delete */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;	/**< Handle delegate of reloaded assets */
};

#endif // !MATERIALEDITORWINDOW_H