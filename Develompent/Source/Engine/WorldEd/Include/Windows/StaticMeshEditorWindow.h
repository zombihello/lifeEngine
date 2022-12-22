/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHEDITORWINDOW_H
#define STATICMESHEDITORWINDOW_H

#include "ImGUI/ImGUIEngine.h"
#include "Render/StaticMesh.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/SelectAssetWidget.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Static mesh editor window
 */
class CStaticMeshEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Enumeration icon types
	 */
	enum EIconType
	{
		IT_Import,		/**< Icon for button 'Import' */
		IT_Num			/**< Number of icon types */
	};

	/**
	 * @brief Constructor
	 * @param InStaticMesh	Static mesh
	 */
	CStaticMeshEditorWindow( const TSharedPtr<CStaticMesh>& InStaticMesh );

	/**
	 * @brief Destructor
	 */
	~CStaticMeshEditorWindow();

	/**
	 * @brief Init
	 */
	virtual void Init() override;

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) override;

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
		uint32								slotId;				/**< Slot ID */
		TAssetHandle<CMaterial>				asset;				/**< Asset */
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

	TAssetHandle<CTexture2D>								icons[IT_Num];			/**< Array of icons */
	TSharedPtr<CStaticMesh>									staticMesh;				/**< Static mesh */
	CViewportWidget											viewportWidget;			/**< Viewport widget */
	class CStaticMeshPreviewViewportClient*					viewportClient;			/**< Viewport client */
	std::vector<SSelectAssetHandle>							selectAssetWidgets;		/**< Array of select asset widgets */
	SEditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;	/**< Handle delegate of assets can delete */
	SEditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;	/**< Handle delegate of reloaded assets */
};

#endif // !STATICMESHEDITORWINDOW_H