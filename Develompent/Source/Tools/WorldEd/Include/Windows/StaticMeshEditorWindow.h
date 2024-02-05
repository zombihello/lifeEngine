/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef STATICMESHEDITORWINDOW_H
#define STATICMESHEDITORWINDOW_H

#include "UI/ImGUI/ImGUIEngine.h"
#include "Engine/Render/StaticMesh.h"
#include "Editor/Editor.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/SelectAssetWidget.h"

/**
 * @ingroup WorldEd
 * @brief Static mesh editor window
 */
class CStaticMeshEditorWindow : public CImGUILayer
{
public:
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
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent ) override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Select asset handle
	 */
	struct SelectAssetHandle
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
	void OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, struct CanDeleteAssetResult& OutResult );

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

	TSharedPtr<CStaticMesh>									staticMesh;				/**< Static mesh */
	CViewportWidget											viewportWidget;			/**< Viewport widget */
	class CStaticMeshPreviewViewportClient*					viewportClient;			/**< Viewport client */
	std::vector<SelectAssetHandle>							selectAssetWidgets;		/**< Array of select asset widgets */
	EditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;	/**< Handle delegate of assets can delete */
	EditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;	/**< Handle delegate of reloaded assets */
};

#endif // !STATICMESHEDITORWINDOW_H